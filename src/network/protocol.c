/**
 * @file protocol.c
 * @brief libwebsockets 的协议
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <libwebsockets.h>

/**
 * 消息结构体
 */
struct msg {
  void *payload;
  size_t len;
};

/**
 * 客户端结构体
 * 本质是一个链表项
 */
struct per_session_data_saws {
  struct per_session_data_saws *pss_list;    // 下一个客户端
  struct lws *wsi;                           // 与这个客户端相关的连接的句柄
  int last;                                  // 当前接收的消息编号
};

/**
 * 服务端（vhost）结构体
 */
struct per_vhost_data_saws {
  struct lws_context *context;               // 服务器上下文
  struct lws_vhost *vhost;                   // 服务器句柄
  const struct lws_protocols *protocol;      // 服务器协议

  struct per_session_data_saws *pss_list;    // 客户端链表

  struct msg amsg;                           // 接收到的消息，缓存大小为一条数据
  int current;                               // 当前消息编号，用来同步所有客户端的消息
};

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
    case LWS_CALLBACK_PROTOCOL_INIT:
      // 初始化服务器
      // 这个回调只会被调用一次
      vhd = lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
                                        lws_get_protocol(wsi),
                                        sizeof(struct per_vhost_data_saws));
      vhd->context = lws_get_context(wsi);
      vhd->protocol = lws_get_protocol(wsi);
      vhd->vhost = lws_get_vhost(wsi);
      break;

    case LWS_CALLBACK_ESTABLISHED:
      // 连接已产生
      // 将当前的客户端加入客户端列表
      saws_log("A client connection established");
      lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
      pss->wsi = wsi;
      pss->last = vhd->current;
      break;

    case LWS_CALLBACK_CLOSED:
      // 连接已断开
      // 删除当前客户端
      lws_ll_fwd_remove(struct per_session_data_saws, pss_list,
                      pss, vhd->pss_list);
      break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
      // 在 lws_callback_on_writeable 时为此回调

      // 消息为空就不发了
      if (!vhd->amsg.payload)
        break;

      // 消息发过就不发了
      if (pss->last == vhd->current)
        break;

      /* notice we allowed for LWS_PRE in the payload already */
      m = lws_write(wsi, ((unsigned char *)vhd->amsg.payload) +
                         LWS_PRE, vhd->amsg.len, LWS_WRITE_TEXT);
      if (m < (int)vhd->amsg.len) {
        lwsl_err("ERROR %d writing to ws\n", m);
        return -1;
      }

      // 发完后更新消息编号
      pss->last = vhd->current;
      break;

    case LWS_CALLBACK_RECEIVE:
      // 收到消息

      // 先清空自己的消息缓存
      if (vhd->amsg.payload)
        __saws_destroy_message(&vhd->amsg);

      // 将收到的消息写入 vhost 的 amsg 域
      vhd->amsg.len = len;
      /* notice we over-allocate by LWS_PRE */
      vhd->amsg.payload = malloc(LWS_PRE + len);
      if (!vhd->amsg.payload) {
        lwsl_user("OOM: dropping\n");
        break;
      }

      memcpy((char *)vhd->amsg.payload + LWS_PRE, in, len);
      vhd->current++;

      // 告知 vhost 的所有客户（i.e. vhd->pss_list），
      // 「我们有消息要发给你们」
      lws_start_foreach_llp(struct per_session_data_saws **, ppss, vhd->pss_list) {
            lws_callback_on_writable((*ppss)->wsi);
      } lws_end_foreach_llp(ppss, pss_list);
      break;

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