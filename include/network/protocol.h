#ifndef SAWS_PROTOCOL_H
#define SAWS_PROTOCOL_H

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
  int client_id;                             // 客户端编号
  int room_id;                               // 与这个客户端绑定的房间号
  int uid;
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
};

#endif //SAWS_PROTOCOL_H
