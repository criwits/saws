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
  NPC_UPLOAD,
  REMOVE_AIRCRAFT,
  PROP_ACTION,
  GAME_END_REQUEST,
  GET_RANKINGS,
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

struct npc_upload_s {
  int mob;
  int id;
  int location_x;
  int location_y;
  int speed_x;
  int speed_y;
  int hp;
};

struct damage_s {
  int id;
  int hp_decrease;
  int location_x;
  int location_y;
};

struct remove_aircraft_s {
  int remove;
};

struct prop_action_s {
  int id;
};

struct game_end_request_s {
  int reason;
};


/** ENCODE **/

enum {
  USER_QUERY_RESPONSE = 0,
  ROOM_INFO_RESPONSE,
  CREATE_ROOM_RESPONSE,
  JOIN_ROOM_RESPONSE,
  ROOM_READY,
  GAME_START,
  NPC_SPAWN,
  TEAMMATE_MOVEMENT,
  SCORE,
  PROP_SPAWN,
  BOMB_ACTION,
  BLOOD_ACTION,
  BULLET_ACTION,
  GAME_END,
  RANKINGS
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

struct game_start_s {
  double ratio;
  int enemy_bullet_power;
  int boss_bullet_power;
};

struct npc_spawn_s {
  int mob;
  int id;
  int location_x;
  int location_y;
  int speed_x;
  int speed_y;
  int hp;
};

struct teammate_movement_s {
  int new_x;
  int new_y;
};

struct score_s {
  int remove;
  int score;
};

struct prop_spawn_s {
  struct prop_spawn_s *next;
  int id;
  int kind;
  int location_x;
  int location_y;
};

struct bomb_action_s {
  int add_score;
};

struct bullet_action_s {
  bool target;
};

struct game_end_s {
  int reason;
  int this_score;
  int teammate_score;
};

void msg_jump();

#endif //SAWS_MSG_H
