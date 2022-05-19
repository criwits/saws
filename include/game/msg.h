#ifndef SAWS_MSG_H
#define SAWS_MSG_H

int decode_msg(const char *msg, void **msg_struct);

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



#endif //SAWS_MSG_H
