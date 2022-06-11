/**
 * @file protocol.c
 * @brief libwebsockets 的协议
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <libwebsockets.h>
#include <game/msg.h>
#include <database/user.h>

#include <network/protocol.h>
#include <game/room.h>
#include <game/logic.h>

#include <setjmp.h>
#include <math.h>

static int client = 0;
static int room_id = 0;
static jmp_buf encode_jmp_buf;

#define write_message(wsi_s, msg_s, type) \
  do {                                        \
    char *msg_buf = encode_msg(msg_s, type); \
    size_t msg_len = strlen(msg_buf);     \
    if (msg_len > 0) {                                      \
      struct msg *new_msg = (struct msg *)malloc(sizeof(struct msg)); \
      new_msg->len = msg_len;             \
      new_msg->payload = malloc(LWS_PRE + (msg_len));                 \
      if (!new_msg->payload) { \
        saws_warn("Out of memory when handling JSON: %s", msg_buf); \
        break; \
      } \
      memcpy((char *)new_msg->payload + LWS_PRE, msg_buf, msg_len); \
      free(msg_buf); \
      new_msg->wsi = wsi_s;  \
      new_msg->next = vhd->msg_query;       \
      if (vhd->msg_query != NULL) {    \
        vhd->msg_query->prev = new_msg;       \
      }  \
      new_msg->prev = NULL; \
      vhd->msg_query = new_msg;           \
      lws_callback_on_writable(wsi_s);      \
    }                                        \
  } while(0);

void clear_prop_spawn_s(struct prop_spawn_s *s) {
  if (s == NULL) {
    return;
  }
  clear_prop_spawn_s(s->next);
  free(s);
}

/**
 * libwebsockets 回调函数
 * @param wsi
 * @param reason
 * @param user
 * @param in
 * @param len
 * @return
 */
int callback_event(struct lws *wsi, enum lws_callback_reasons reason,
                   void *user, void *in, size_t len) {
  struct per_session_data_saws *pss =
      (struct per_session_data_saws *)user;
  struct per_vhost_data_saws *vhd =
      (struct per_vhost_data_saws *)
          lws_protocol_vh_priv_get(lws_get_vhost(wsi),
                                   lws_get_protocol(wsi));
  int m;

  switch (reason) {
    case LWS_CALLBACK_PROTOCOL_INIT: {
      // 初始化服务器
      // 这个回调只会被调用一次
      vhd = lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
                                        lws_get_protocol(wsi),
                                        sizeof(struct per_vhost_data_saws));
      vhd->context = lws_get_context(wsi);
      vhd->protocol = lws_get_protocol(wsi);
      vhd->vhost = lws_get_vhost(wsi);
      vhd->msg_query = NULL;
      saws_log("Server initialised");
      break;
    }

    case LWS_CALLBACK_ESTABLISHED: {
      // 连接已产生
      // 将当前的客户端加入客户端列表
      saws_log("A client connection established with id %d", client);
      lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
      pss->wsi = wsi;
      pss->client_id = client++;
      pss->uid = -1;
      break;
    }

    case LWS_CALLBACK_CLOSED: {
      // 连接已断开
      // 删除当前客户端
      saws_log("Client %d lost its connection", pss->client_id);
      lws_ll_fwd_remove(struct per_session_data_saws, pss_list,
                        pss, vhd->pss_list);
      if (pss->room != NULL) {
        stop_game(pss->room);
      }
      break;
    }

    case LWS_CALLBACK_SERVER_WRITEABLE: {
      // 给当前回调的客户端发送消息
      if (vhd->msg_query != NULL && vhd->msg_query->next == vhd->msg_query) {
        saws_warn("Loop detected, emergency clear buffer");
        free(vhd->msg_query);
        vhd->msg_query = NULL;
        break;
      }
      for (struct msg *ptr = vhd->msg_query; ptr != NULL;) {
        if (ptr->wsi == wsi && ptr->payload != NULL) {
          m = lws_write(wsi, ((unsigned char *) ptr->payload) +
                                  LWS_PRE, ptr->len, LWS_WRITE_TEXT);
          size_t msg_len = ptr->len;
          struct msg *next = ptr->next;

          // 删除
          if (ptr->prev == NULL && ptr->next == NULL) {
            vhd->msg_query = NULL;
          } else if (ptr->prev == NULL) {
            ptr->next->prev = NULL;
            vhd->msg_query = vhd->msg_query->next;
          } else if (ptr->next == NULL) {
            ptr->prev->next = NULL;
          } else {
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
          }
          free(ptr->payload);
          free(ptr);

          if (m < (int) msg_len) {
            saws_err("Error %d occurred while writing to client", m);
            return -1;
          }
          ptr = next;
          // break;
        } else {
          ptr = ptr->next;
        }
      }
      break;
    }

    case LWS_CALLBACK_RECEIVE: {
      void *msg_struct_raw = NULL;
      // char *message_buffer = (char *) malloc(len);
      // memcpy(message_buffer, in, len);
      // int msg_type = decode_msg(message_buffer, &msg_struct_raw);
      if (setjmp(encode_jmp_buf)) {
        saws_warn("Error occurred and callback was broken");
        break;
      }
      int msg_type = decode_msg(in, &msg_struct_raw);
      switch (msg_type) {
        case USER_QUERY: {
          struct user_query_s *msg_struct = (struct user_query_s *)msg_struct_raw;
          saws_debug("User query for (%s, %c***)", msg_struct->username, *(msg_struct->password));
          int uid = query_user(msg_struct->username, msg_struct->password);
          if (uid == -1) {
            saws_debug("Invalid username or password");
          } else {
            saws_debug("Successfully logged in with UID: %d", uid);
            // 将用户和连接绑定
            pss->uid = uid;
          }
          // 返回登录状态
          struct user_query_response_s msg = {
              .uid = uid
          };
          write_message(pss->wsi, &msg, USER_QUERY_RESPONSE)
          free(msg_struct);
          break;
        }

        case ROOM_INFO: {
          write_message(pss->wsi, NULL, ROOM_INFO_RESPONSE)
          break;
        }

        case CREATE_ROOM: {
          struct create_room_s *msg_struct = (struct create_room_s *)msg_struct_raw;
          saws_debug("Client %d trying to create a room with difficulty level %d", pss->client_id, msg_struct->difficulty);
          // 开房
          int curr_room_id = room_id++;
          // 将房间与客户端绑定
          pss->room = add_room(curr_room_id, pss, NULL, vhd);
          pss->room->host = pss;
          pss->room->host_uid = pss->uid;
          pss->room->difficulty = msg_struct->difficulty;

          struct create_room_response_s msg = {
              .room_id = pss->room->room_id
          };
          write_message(pss->wsi, &msg, CREATE_ROOM_RESPONSE)

          free(msg_struct);
          break;
        }

        case JOIN_ROOM: {
          struct join_room_s *msg_struct = (struct join_room_s *)msg_struct_raw;
          saws_debug("Client %d trying to join room id %d", pss->client_id, msg_struct->room_id);
          room_t *room = get_room_by_id(msg_struct->room_id);
          struct join_room_response_s msg;
          if (room == NULL) {
            saws_debug("Invalid room id %d", msg_struct->room_id);
            msg.success = false;
          } else {
            if (room->guest != NULL) {
              msg.success = false;
              saws_debug("Room %d is unavailable", msg_struct->room_id);
            } else {
              // 可以加入房间
              msg.success = true;
              saws_debug("Client %d successfully joined room %d", pss->client_id, room->room_id);
              room->guest = pss;
              room->guest_uid = pss->uid;
              pss->room = room;
              write_message(room->host->wsi, NULL, ROOM_READY);
            }
          }
          write_message(pss->wsi, &msg, JOIN_ROOM_RESPONSE);

          free(msg_struct);
          break;
        }

        case RESOLUTION: {
          struct resolution_s *msg_struct = (struct resolution_s *)msg_struct_raw;
          saws_debug("Client %d reported its screen resolution %d x %d", pss->client_id, msg_struct->width, msg_struct->height);
          // TODO: check if client has entered a room
          double real_ratio;
          if (pss->uid == pss->room->host_uid) {
            // 房主
            pss->room->host_width = msg_struct->width;
            pss->room->host_height = msg_struct->height;
          } else {
            // 房客
            pss->room->guest_width = msg_struct->width;
            pss->room->guest_height = msg_struct->height;
          }
          if (pss->room->host_width != -1 && pss->room->guest_width != -1) {
            // 此时，双方都已经上报屏幕信息
            double host_ratio = pss->room->host_height / (double) pss->room->host_width;
            double guest_ratio = pss->room->guest_height / (double) pss->room->guest_width;
            real_ratio = host_ratio > guest_ratio ? guest_ratio : host_ratio;

            // 开始游戏
            struct game_start_s msg = {
                .ratio = real_ratio,
            };

            write_message(pss->room->host->wsi, &msg, GAME_START)
            write_message(pss->room->guest->wsi, &msg, GAME_START)

            start_game(pss->room);
          }

          free(msg_struct);
          break;
        }

        case MOVEMENT: if (get_game_status(pss->room)) {
          // 游戏双方之任何一方发来「移动」消息，
          // 都转发到另一边。
          struct movement_s *msg_struct = (struct movement_s *)msg_struct_raw;
          struct teammate_movement_s msg = {
              .new_x = msg_struct->new_x,
              .new_y = msg_struct->new_y
          };
          if (pss->uid == pss->room->host_uid) {
            write_message(pss->room->guest->wsi, &msg, TEAMMATE_MOVEMENT)
          } else {
            write_message(pss->room->host->wsi, &msg, TEAMMATE_MOVEMENT)
          }

          free(msg_struct);
          break;
        }

        case NPC_UPLOAD: if (get_game_status(pss->room)) {
          struct npc_upload_s *msg_struct = (struct npc_upload_s *)msg_struct_raw;
          struct npc_spawn_s msg = {
              .mob = msg_struct->mob,
              .id = msg_struct->id,
              .location_x = msg_struct->location_x,
              .location_y = msg_struct->location_y,
              .speed_x = msg_struct->speed_x,
              .speed_y = msg_struct->speed_y,
              .hp = msg_struct->hp
          };

          add_npc(msg.id, msg.hp, msg.mob, pss->room);
          saws_debug_room("Spawned NPC with id %d", pss->room->room_id, msg.id);

          write_message(pss->room->guest->wsi, &msg, NPC_SPAWN)
          free(msg_struct);
          break;
        }

        case REMOVE_AIRCRAFT: if (get_game_status(pss->room)) {
          struct remove_aircraft_s *msg_struct = (struct remove_aircraft_s *) msg_struct_raw;
          remove_npc(msg_struct->remove, pss->room);
          saws_debug_room("Removed NPC %d", pss->room->room_id, msg_struct->remove);
          free(msg_struct);
          break;
        }

        case DAMAGE: if (get_game_status(pss->room)) {
          struct damage_s *msg_struct = (struct damage_s *) msg_struct_raw;
          aircraft_t *aircraft = get_npc(msg_struct->id, pss->room);

          if (aircraft != NULL) {
            // 扣血
            aircraft->hp -= (msg_struct->hp_decrease);

            if (aircraft->hp <= 0) {
              // 加分
              int score;
              switch (aircraft->mob) {
                case 0:
                  score = 10;
                  break;
                case 1:
                  score = 20;
                  break;
                case 2:
                  score = 50;
                  break;
                default:
                  score = 0;
                  break;
              }
              saws_debug_room("Client %d kills NPC with id %d and get score %d", pss->room->room_id, pss->client_id, aircraft->id, score);
              if (pss->wsi == pss->room->host->wsi) {
                // 当前是房主
                pss->room->host_score += score;
                struct score_s host = {
                    .remove = msg_struct->id,
                    .score = score
                };
                struct score_s guest = {
                    .remove = msg_struct->id,
                    .score = 0
                };
                write_message(pss->room->host->wsi, &host, SCORE)
                write_message(pss->room->guest->wsi, &guest, SCORE)
              } else {
                // 当前是房客
                pss->room->guest_score += score;
                struct score_s host = {
                    .remove = msg_struct->id,
                    .score = 0
                };
                struct score_s guest = {
                    .remove = msg_struct->id,
                    .score = score
                };
                write_message(pss->room->host->wsi, &host, SCORE)
                write_message(pss->room->guest->wsi, &guest, SCORE)
              }
              // 发放道具
              srand(time(0));
              int prop_cnt = 0;
              switch (aircraft->mob) {
                case 0:
                  prop_cnt = 0;
                  break;
                case 1:
                  prop_cnt = 1;
                  break;
                case 2:
                  prop_cnt = 3;
                  break;
                default:
                  break;
              }
              struct prop_spawn_s *props = NULL;
              for (int i = 0; i < prop_cnt; i++) {
                int factor = rand() % 10; // [0, 9]
                if (0 <= factor && factor < 6) {
                  int kind;
                  if (0 <= factor && factor < 2) {
                    // Blood
                    kind = 0;
                  } else if (2 <= factor && factor < 4) {
                    // Bomb
                    kind = 1;
                  } else if (4 <= factor && factor < 6) {
                    // Bullet
                    kind = 2;
                  }
                  struct prop_spawn_s *prop = (struct prop_spawn_s *) malloc(sizeof(struct prop_spawn_s));
                  prop->next = props;
                  prop->id = pss->room->prop_id;
                  prop->location_x = msg_struct->location_x + rand() % 20 - 10;
                  prop->location_y = msg_struct->location_y + rand() % 20 - 10;
                  prop->kind = kind;
                  props = prop;

                  add_prop(prop->id, prop->kind, pss->room);
                  saws_debug_room("Generated prop id %d (kind: %d) at (%d, %d)", pss->room->room_id,
                                  prop->id, prop->kind, prop->location_x, prop->location_y);
                  pss->room->prop_id++;
                }
              }
              write_message(pss->room->host->wsi, props, PROP_SPAWN)
              write_message(pss->room->guest->wsi, props, PROP_SPAWN)
              clear_prop_spawn_s(props);

              // 删除飞机
              // remove_npc(msg_struct->id, pss->room);
              remove_given_npc(aircraft, pss->room);
            }
          }

          free(msg_struct);
          break;
        }

        case PROP_ACTION: if (get_game_status(pss->room)) {
          struct prop_action_s *msg_struct = (struct prop_action_s *) msg_struct_raw;
          prop_t *prop = get_prop(msg_struct->id, pss->room);
          if (prop != NULL) {
            switch (prop->kind) {
              case 0: {
                // 加血道具
                write_message(pss->wsi, NULL, BLOOD_ACTION);
                saws_debug_room("Client %d triggered blood prop %d",
                                pss->room->room_id, pss->client_id, msg_struct->id);
                break;
              }

              case 1: {
                // 炸弹道具
                int score = remove_and_score_all_npcs(pss->room);
                if (pss->wsi == pss->room->host->wsi) {
                  // 当前是房主
                  pss->room->host_score += score;
                  struct bomb_action_s host = {
                      .add_score = score
                  };
                  struct bomb_action_s guest = {
                      .add_score = 0
                  };
                  write_message(pss->room->host->wsi, &host, BOMB_ACTION)
                  write_message(pss->room->guest->wsi, &guest, BOMB_ACTION)
                } else {
                  // 当前是房客
                  pss->room->guest_score += score;
                  struct bomb_action_s host = {
                      .add_score = 0
                  };
                  struct bomb_action_s guest = {
                      .add_score = score
                  };
                  write_message(pss->room->host->wsi, &host, BOMB_ACTION)
                  write_message(pss->room->guest->wsi, &guest, BOMB_ACTION)
                }
                saws_debug_room("Client %d triggered bomb prop %d (score: %d)",
                                pss->room->room_id, pss->client_id, msg_struct->id, score);
                break;
              }

              case 2: {
                // 火力道具
                if (pss->wsi == pss->room->host->wsi) {
                  // 当前是房主
                  struct bullet_action_s host = {
                      .target = true
                  };
                  struct bullet_action_s guest = {
                      .target = false
                  };
                  write_message(pss->room->host->wsi, &host, BULLET_ACTION)
                  write_message(pss->room->guest->wsi, &guest, BULLET_ACTION)
                } else {
                  // 当前是房客
                  struct bullet_action_s host = {
                      .target = false
                  };
                  struct bullet_action_s guest = {
                      .target = true
                  };
                  write_message(pss->room->host->wsi, &host, BULLET_ACTION)
                  write_message(pss->room->guest->wsi, &guest, BULLET_ACTION)
                }

                saws_debug_room("Client %d triggered bullet prop %d",
                                pss->room->room_id, pss->client_id, msg_struct->id);
                break;
              }
              default:
                break;
            }
            // 删除当前道具，因为它已经生效
            // remove_prop(msg_struct->id, pss->room);
            remove_given_prop(prop, pss->room);
          }

          free(msg_struct);
          break;
        }

        case GAME_END_REQUEST: if (get_game_status(pss->room)) {
          struct game_end_request_s *msg_struct = (struct game_end_request_s *) msg_struct_raw;
          // 游戏结束
          saws_debug_room("Client %d request end game for reason %d",
                          pss->room->room_id, pss->client_id, msg_struct->reason);
          // 通知双方
          struct game_end_s host = {
              .reason = msg_struct->reason,
              .this_score = pss->room->host_score,
              .teammate_score = pss->room->guest_score
          };
          struct game_end_s guest = {
              .reason = msg_struct->reason,
              .this_score = pss->room->guest_score,
              .teammate_score = pss->room->host_score
          };
          write_message(pss->room->host->wsi, &host, GAME_END)
          write_message(pss->room->guest->wsi, &guest, GAME_END)
          // 关闭房间
          stop_game(pss->room);
          free(msg_struct);
          break;
        }

        default: {
          saws_warn("Unhandled message type");
          break;
        }
      }

      // free(message_buffer);
      break;
    }

    default:
      break;
  }

  return 0;
}

/**
 * 用于 JSON 解析出现异常时的跳转
 */
void msg_jump() {
  longjmp(encode_jmp_buf, -1);
}

