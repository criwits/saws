/**
 * @file net_event.h
 * @author hans
 * @version 0.1
 */
#ifndef SAWS_NET_PROTOCOL_H
#define SAWS_NET_PROTOCOL_H

#include <common.h>

#define LWS_PLUGIN_PROTOCOL_SAWS \
  { \
    "saws", \
    callback_event, \
    sizeof(struct per_session_data_saws), \
    512 * 1024, \
    0, NULL, 512 * 1024 \
  }

int callback_event(struct lws *wsi, enum lws_callback_reasons reason,
                   void *user, void *in, size_t len);

#endif //SAWS_NET_PROTOCOL_H
