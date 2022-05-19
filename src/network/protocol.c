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

static int client = 0;
static int room_id = 0;


/**
 * 释放消息
 * @param _msg
 */
static void __saws_destroy_message(void *_msg) {
  struct msg *msg = _msg;

  free(msg->payload);
  msg->payload = NULL;
  msg->len = 0;
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
static int callback_saws(struct lws *wsi, enum lws_callback_reasons reason,
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
      break;
    }

    case LWS_CALLBACK_ESTABLISHED: {
      // 连接已产生
      // 将当前的客户端加入客户端列表
      saws_log("A client connection established with id %d", client);
      lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
      pss->wsi = wsi;
      pss->client_id = client++;
      pss->room_id = -1;
      pss->uid = -1;
      break;
    }

    case LWS_CALLBACK_CLOSED: {
      // 连接已断开
      // 删除当前客户端
      saws_log("Client %d lost its connection", pss->client_id);
      lws_ll_fwd_remove(struct per_session_data_saws, pss_list,
                        pss, vhd->pss_list);
      break;
    }

    case LWS_CALLBACK_SERVER_WRITEABLE: {
      // 在 lws_callback_on_writeable 时为此回调


      /* notice we allowed for LWS_PRE in the payload already */
      m = lws_write(wsi, ((unsigned char *) vhd->amsg.payload) +
                         LWS_PRE, vhd->amsg.len, LWS_WRITE_TEXT);
      if (m < (int) vhd->amsg.len) {
        lwsl_err("ERROR %d writing to ws\n", m);
        return -1;
      }

      break;
    }

    case LWS_CALLBACK_RECEIVE: {
      void *msg_struct_raw = NULL;
      int msg_type = decode_msg(in, &msg_struct_raw);
      switch (msg_type) {
        case USER_QUERY: {
          struct user_query_s *msg_struct = (struct user_query_s *)msg_struct_raw;
          saws_debug("User query for (%s, %s)", msg_struct->username, msg_struct->password);
          int uid = query_user(msg_struct->username, msg_struct->password);
          if (uid == -1) {
            saws_debug("Invalid username or password");
            // TODO
          } else {
            saws_debug("Successfully logged in with UID: %d", uid);
            // TODO
          }

          break;
        }

        default: {
          break;
        }
      }
    }

//      // 先清空自己的消息缓存
//      if (vhd->amsg.payload)
//        __saws_destroy_message(&vhd->amsg);
//
//      // 将收到的消息写入 vhost 的 amsg 域
//      vhd->amsg.len = len;
//      /* notice we over-allocate by LWS_PRE */
//      vhd->amsg.payload = malloc(LWS_PRE + len);
//      if (!vhd->amsg.payload) {
//        lwsl_user("OOM: dropping\n");
//        break;
//      }
//
//      memcpy((char *)vhd->amsg.payload + LWS_PRE, in, len);
//      vhd->current++;
//
//      // 告知 vhost 的所有客户（i.e. vhd->pss_list），
//      // 「我们有消息要发给你们」
//      lws_start_foreach_llp(struct per_session_data_saws **, ppss, vhd->pss_list) {
//            lws_callback_on_writable((*ppss)->wsi);
//      } lws_end_foreach_llp(ppss, pss_list);
//      break;

    default:
      break;
  }

  return 0;
}

#define LWS_PLUGIN_PROTOCOL_SAWS \
        { \
                "saws", \
                callback_saws, \
                sizeof(struct per_session_data_saws), \
                1024, \
                0, NULL, 0 \
        }