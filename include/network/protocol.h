#ifndef SAWS_PROTOCOL_H
#define SAWS_PROTOCOL_H

#include <common.h>
#include <libwebsockets.h>

#include <game/room.h>

/**
 * 消息结构体
 */
struct msg {
  void *payload;
  size_t len;
  struct lws *wsi;
  struct msg *next;
};

/**
 * 客户端结构体
 * 本质是一个链表项
 */
struct per_session_data_saws {
  struct per_session_data_saws *pss_list;    // 下一个客户端
  struct lws *wsi;                           // 与这个客户端相关的连接的句柄
  int client_id;                             // 客户端编号
  int uid;
  // 好像因为头文件互相包含了，不能用 room_t
  struct room_s *room;
};

/**
 * 服务端（vhost）结构体
 */
struct per_vhost_data_saws {
  struct lws_context *context;               // 服务器上下文
  struct lws_vhost *vhost;                   // 服务器句柄
  const struct lws_protocols *protocol;      // 服务器协议

  struct per_session_data_saws *pss_list;    // 客户端链表
  struct msg *msg_query;                     // 消息缓存
  int msg_cnt;
};

#endif //SAWS_PROTOCOL_H
