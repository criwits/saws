#ifndef SAWS_MSG_H
#define SAWS_MSG_H

int decode_msg(const char *msg, void **msg_struct);
char *encode_msg(const void *msg_struct, int type);

/** DECODE **/

enum {
  USER_QUERY = 0,
  ROOM_INFO,
  CREATE_ROOM,
  JOIN_ROOM,
  RESOLUTION,
  MOVEMENT,
  DAMAGE,
  RECV_MSG_CNT
};

struct user_query_s {
#define MAX_USERNAME_LENGTH 128
#define MAX_PASSWORD_LENGTH 128
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
};

struct create_room_s {
  int difficulty;
};

struct join_room_s {
  int room_id;
};

struct resolution_s {
  int width;
  int height;
};

struct movement_s {
  int new_x;
  int new_y;
};

struct damage_s {
  int id;
  int hp_decrease;
};

/** ENCODE **/

enum {
  USER_QUERY_RESPONSE = 0,
  ROOM_INFO_RESPONSE,
  CREATE_ROOM_RESPONSE,
  JOIN_ROOM_RESPONSE,
  ROOM_READY
};

struct user_query_response_s {
  int uid;
};

struct create_room_response_s {
  int room_id;
};

struct join_room_response_s {
  bool success;
};

#endif //SAWS_MSG_H
