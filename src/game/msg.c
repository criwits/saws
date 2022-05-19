/**
 * @file msg.c
 * @brief 消息处理器
 * @author hans
 * @version 0.1
 */

const char *msg_type[] = {
    "user_query",
    "room_info",
    "create_room",
    "join_room",
    "resolution",
    "movement",
    "damage"
};


#include <common.h>
#include <game/msg.h>
#include <utils/json_macro.h>

typedef void (*msg_handler_t)(cJSON *json_node(root), void **msg_struct);
#define def_msg_handler(pattern) static void pattern(cJSON *json_node(root), void **msg_struct)

def_msg_handler(user_query) {
  *msg_struct = malloc(sizeof(struct user_query_s));
  struct user_query_s *s = (struct user_query_s *)(*msg_struct);
  json_parse_node(root, username)
  json_parse_node(root, password)
  strcpy(s->username, json_node(username)->valuestring);
  strcpy(s->password, json_node(password)->valuestring);
}


msg_handler_t msg_handler[] = {
    user_query, NULL
};

int decode_msg(const char *msg, void **msg_struct) {
  cJSON *json_node(root) = cJSON_Parse(msg);
  json_parse_node(root, type)
  int type = 0;
  for (int i = 0; i < RECV_MSG_CNT; i++) {
    if (0 == strcmp(json_node(type)->valuestring, msg_type[i])) {
      type = i;
      break;
    }
  }
  msg_handler[type](json_node(root), msg_struct);
  cJSON_Delete(json_node(root));
  return type;
}

