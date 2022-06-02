/**
 * @file send.h
 * @brief 异步通信函数
 * @author hans
 * @version 0.1
 */
#ifndef SAWS_SEND_H
#define SAWS_SEND_H

#include <network/protocol.h>

void send_message(struct per_vhost_data_saws *vhd, struct lws *wsi, const void *msg, int type);

#endif //SAWS_SEND_H
