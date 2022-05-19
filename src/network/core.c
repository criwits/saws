/**
 * @file core.c
 * @brief libwebsockets 服务器核心
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <libwebsockets.h>
#include <engine/server.h>

#include "protocol.c"

static bool interrupted = false;
static struct lws_context_creation_info info;
static struct lws_context *context;

static struct lws_protocols protocols[] = {
    LWS_PLUGIN_PROTOCOL_SAWS,
    {NULL, NULL, 0}
};

int lws_loop() {
  int n = 0;
  while (n >= 0 && !interrupted) {
    n = lws_service(context, 0);
  }
  return n;
}

void lws_init(int port) {
  memset(&info, 0, sizeof info);
  info.port = port;
  info.protocols = protocols;
  info.pvo = NULL;
  info.pt_serv_buf_size = 32 * 1024;
  info.options = LWS_SERVER_OPTION_VALIDATE_UTF8 |
                 LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
  context = lws_create_context(&info);
  if (!context) {
    saws_err("Cannot initialise WebSocket server");
    stop_server(-1);
  }
  saws_log("Successfully initialised WebSocket server on port %d", port);
}

void lws_stop() {
  saws_log("Stopped WebSocket server");
  interrupted = true;
  lws_context_destroy(context);
}