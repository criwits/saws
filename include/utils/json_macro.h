#ifndef SAWS_JSON_PARSE_H
#define SAWS_JSON_PARSE_H

#include "common/common.h"
#include "cJSON.h"

#define json_node(item) concat(item, _json_node)
#define json_array(arr) concat(arr, _json_array)

#define json_parse_node(parent, item) \
  cJSON *json_node(item); \
  if ((json_node(item) = cJSON_GetObjectItem(json_node(parent), str(item))) == NULL) { \
    saws_warn("Cannot parse `%s` field in given JSON", str(item));                     \
    cJSON_Delete(json_node(root)); \
    msg_jump(); \
  }



#endif //SAWS_JSON_PARSE_H
