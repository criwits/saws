/**
 * @file msg.c
 * @brief 消息处理器
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <game/msg.h>
#include <utils/json_macro.h>
#include <game/room.h>

#include <setjmp.h>

/** DECODE **/

const char *msg_recv_type[] = {
    "user_query",
    "room_info",
    "create_room",
    "join_room",
    "resolution",
    "movement",
    "damage",
    "npc_upload",
    "remove_aircraft"
};

typedef void (*msg_handler_t)(cJSON *json_node(root), void **msg_struct);
#define def_msg_handler(pattern) static inline void pattern(cJSON *json_node(root), void **msg_struct)

def_msg_handler(user_query) {
  *msg_struct = malloc(sizeof(struct user_query_s));
  struct user_query_s *s = (struct user_query_s *)(*msg_struct);
  json_parse_node(root, username)
  json_parse_node(root, password)
  strcpy(s->username, json_node(username)->valuestring);
  strcpy(s->password, json_node(password)->valuestring);
}

def_msg_handler(room_info) {
  // 空的
}

def_msg_handler(create_room) {
  *msg_struct = malloc(sizeof(struct create_room_s));
  struct create_room_s *s = (struct create_room_s *)(*msg_struct);
  json_parse_node(root, difficulty)
  s->difficulty = json_node(difficulty)->valueint;
}

def_msg_handler(join_room) {
  *msg_struct = malloc(sizeof(struct join_room_s));
  struct join_room_s *s = (struct join_room_s *)(*msg_struct);
  json_parse_node(root, room_id)
  s->room_id = json_node(room_id)->valueint;
}

def_msg_handler(resolution) {
  *msg_struct = malloc(sizeof(struct resolution_s));
  struct resolution_s *s = (struct resolution_s *)(*msg_struct);
  json_parse_node(root, width)
  json_parse_node(root, height)
  s->width = json_node(width)->valueint;
  s->height = json_node(height)->valueint;
}

def_msg_handler(movement) {
  *msg_struct = malloc(sizeof(struct movement_s));
  struct movement_s *s = (struct movement_s *)(*msg_struct);
  json_parse_node(root, new_x)
  json_parse_node(root, new_y)
  s->new_x = json_node(new_x)->valueint;
  s->new_y = json_node(new_y)->valueint;
}

def_msg_handler(damage) {
  *msg_struct = malloc(sizeof(struct damage_s));
  struct damage_s *s = (struct damage_s *)(*msg_struct);
  json_parse_node(root, id)
  json_parse_node(root, hp_decrease)
  s->hp_decrease = json_node(hp_decrease)->valueint;
  s->id = json_node(id)->valueint;
}

def_msg_handler(npc_upload) {
  *msg_struct = malloc(sizeof(struct npc_upload_s));
  struct npc_upload_s *s = (struct npc_upload_s *)(*msg_struct);
  json_parse_node(root, mob)
  json_parse_node(root, id)
  json_parse_node(root, location_x)
  json_parse_node(root, location_y)
  json_parse_node(root, speed_x)
  json_parse_node(root, speed_y)
  json_parse_node(root, hp)
  s->mob = json_node(mob)->valueint;
  s->id = json_node(id)->valueint;
  s->location_x = json_node(location_x)->valueint;
  s->location_y = json_node(location_y)->valueint;
  s->speed_x = json_node(speed_x)->valueint;
  s->speed_y = json_node(speed_y)->valueint;
  s->hp = json_node(hp)->valueint;
}

def_msg_handler(remove_aircraft) {
  *msg_struct = malloc(sizeof(struct remove_aircraft_s));
  struct remove_aircraft_s *s = (struct remove_aircraft_s *)(*msg_struct);
  json_parse_node(root, remove)
  s->remove = json_node(remove)->valueint;
}

msg_handler_t msg_handler[] = {
    user_query, room_info, create_room, join_room,
    resolution, movement, damage, npc_upload, remove_aircraft,
    NULL
};

int decode_msg(const char *msg, void **msg_struct) {
  cJSON *json_node(root) = cJSON_Parse(msg);
  if (json_node(root) == NULL) {
    saws_warn("Cannot parse given string as a JSON:");
    saws_warn("%s", msg);
  }
  json_parse_node(root, type)
  if (json_node(type) == NULL) {
    saws_warn("%s", msg);
    cJSON_Delete(json_node(root));
    return RECV_MSG_CNT;
  }
  int type = 0;
  for (int i = 0; i < RECV_MSG_CNT; i++) {
    if (0 == strcmp(json_node(type)->valuestring, msg_recv_type[i])) {
      type = i;
      break;
    }
  }
  msg_handler[type](json_node(root), msg_struct);
  cJSON_Delete(json_node(root));
  return type;
}

/** ENCODE **/

typedef void (*msg_decoder_t)(cJSON *json_node(root), const void *msg_struct);
#define def_msg_encoder(pattern) static inline void pattern(cJSON *json_node(root), const void *msg_struct)

def_msg_encoder(user_query_response) {
  struct user_query_response_s *s = (struct user_query_response_s *)msg_struct;
  cJSON_AddStringToObject(json_node(root), "type", "user_query_response");
  cJSON_AddNumberToObject(json_node(root), "uid", s->uid);
}

def_msg_encoder(room_info_response) {
  cJSON_AddStringToObject(json_node(root), "type", "room_info_response");
  cJSON *json_node(rooms) = cJSON_CreateArray();
  for(room_t *room = get_room(); room != NULL; room = room->next) {
    if (room->host_uid != -1 && room->guest_uid != -1) {
      continue;
    }
    cJSON *json_node(room_item) = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_node(room_item), "room_id", room->room_id);
    cJSON_AddNumberToObject(json_node(room_item), "difficulty", room->difficulty);
    cJSON_AddItemToArray(json_node(rooms), json_node(room_item));
  }

  cJSON_AddItemToObject(json_node(root), "rooms", json_node(rooms));
}

def_msg_encoder(create_room_response) {
  struct create_room_response_s *s = (struct create_room_response_s *)msg_struct;
  cJSON_AddStringToObject(json_node(root), "type", "create_room_response");
  cJSON_AddNumberToObject(json_node(root), "room_id", s->room_id);
}

def_msg_encoder(join_room_response) {
  struct join_room_response_s *s = (struct join_room_response_s *)msg_struct;
  cJSON_AddStringToObject(json_node(root), "type", "join_room_response");
  cJSON_AddBoolToObject(json_node(root), "success", s->success);
}

def_msg_encoder(room_ready) {
  cJSON_AddStringToObject(json_node(root), "type", "room_ready");
}

def_msg_encoder(game_start) {
  struct game_start_s *s = (struct game_start_s *)msg_struct;
  cJSON_AddStringToObject(json_node(root), "type", "game_start");
  cJSON_AddNumberToObject(json_node(root), "ratio", s->ratio);
  cJSON_AddNumberToObject(json_node(root), "enemy_bullet_power", s->enemy_bullet_power);
  cJSON_AddNumberToObject(json_node(root), "boss_bullet_power", s->boss_bullet_power);
}

def_msg_encoder(npc_spawn) {
  struct npc_spawn_s *s = (struct npc_spawn_s *)msg_struct;
  cJSON_AddStringToObject(json_node(root), "type", "npc_spawn");
  cJSON_AddNumberToObject(json_node(root), "mob", s->mob);
  cJSON_AddNumberToObject(json_node(root), "id", s->id);
  cJSON_AddNumberToObject(json_node(root), "location_x", s->location_x);
  cJSON_AddNumberToObject(json_node(root), "location_y", s->location_y);
  cJSON_AddNumberToObject(json_node(root), "speed_x", s->speed_x);
  cJSON_AddNumberToObject(json_node(root), "speed_y", s->speed_y);
  cJSON_AddNumberToObject(json_node(root), "hp", s->hp);
}

def_msg_encoder(teammate_movement) {
  struct teammate_movement_s *s = (struct teammate_movement_s *)msg_struct;
  cJSON_AddStringToObject(json_node(root), "type", "teammate_movement");
  cJSON_AddNumberToObject(json_node(root), "new_x", s->new_x);
  cJSON_AddNumberToObject(json_node(root), "new_y", s->new_y);
}

def_msg_encoder(score) {
  struct score_s *s = (struct score_s *)msg_struct;
  cJSON_AddStringToObject(json_node(root), "type", "score");
  cJSON_AddNumberToObject(json_node(root), "remove", s->remove);
  cJSON_AddNumberToObject(json_node(root), "score", s->score);
}

msg_decoder_t msg_decoder[] = {
    user_query_response,
    room_info_response,
    create_room_response,
    join_room_response,
    room_ready,
    game_start,
    npc_spawn,
    teammate_movement,
    score,
    NULL
};

char *encode_msg(const void *msg_struct, int type) {
  cJSON *json_node(root) = cJSON_CreateObject();
  msg_decoder[type](json_node(root), msg_struct);
  char *ret =  cJSON_Print(json_node(root));
  cJSON_Delete(json_node(root));
  return ret;
}