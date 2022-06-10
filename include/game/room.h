#ifndef SAWS_ROOM_H
#define SAWS_ROOM_H

#include <common.h>
#include <libwebsockets.h>

#include <network/protocol.h>
#include "aircraft.h"
#include "prop.h"

typedef struct room_s {
  struct room_s *next;
  int room_id;
  bool running;
  
  int host_uid;
  int guest_uid;

  struct per_session_data_saws *host;
  struct per_session_data_saws *guest;
  struct per_vhost_data_saws *vhost;

  int host_width;
  int host_height;
  int guest_width;
  int guest_height;

  double ratio;

  int difficulty;
  bool has_boss;
  int boss_hp_increase;
  int enemy_max_number;
  int enemy_bullet_power;
  int boss_bullet_power;
  int enemy_speed_y_increase;
  int enemy_speed_y;
  int enemy_speed_y_max;

  int npc_cnt;
  aircraft_t *npc_list;
  int prop_id;
  int prop_cnt;
  prop_t *prop_list;
} room_t;

room_t *add_room(int room_id, struct per_session_data_saws *host, struct per_session_data_saws *guest,
                 struct per_vhost_data_saws *vhost);
room_t *get_room_by_id(int room_id);
void remove_room_by_id(int room_id);
room_t *get_room();
int get_room_cnt();

#endif //SAWS_ROOM_H
