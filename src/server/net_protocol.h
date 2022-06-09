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
    callback_saws, \
    sizeof(struct per_session_data_saws), \
    16384, \
    0, NULL, 16384 \
  }

int callback_saws(struct lws *wsi, enum lws_callback_reasons reason,
                  void *user, void *in, size_t len);

#endif //SAWS_NET_PROTOCOL_H
