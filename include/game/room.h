#ifndef SAWS_ROOM_H
#define SAWS_ROOM_H

#include <common.h>
#include <libwebsockets.h>

#include <network/protocol.h>

typedef struct room_s {
  struct room_s *next;
  int room_id;
  
  int host_uid;
  int guest_uid;
  int timer_id;

  struct per_session_data_saws *host;
  struct per_session_data_saws *guest;
  struct per_vhost_data_saws *vhost;

  int host_width;
  int host_height;
  int guest_width;
  int guest_height;

} room_t;

room_t *add_room(int room_id, struct per_session_data_saws *host, struct per_session_data_saws *guest,
                 struct per_vhost_data_saws *vhost);
room_t *get_room_by_id(int room_id);
void remove_room_by_id(int room_id);
room_t *get_room();
int get_room_cnt();

#endif //SAWS_ROOM_H
