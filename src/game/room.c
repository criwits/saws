/**
 * @file room.c
 * @brief 房间管理
 * @author hans
 * @version 0.1
 */

#include <common.h>

#include <game/room.h>
#include <network/protocol.h>

static room_t *room = NULL;
static int room_cnt = 0;

room_t *add_room(int room_id, struct per_session_data_saws *host, struct per_session_data_saws *guest,
    struct per_vhost_data_saws *vhost) {
  room_t *new_room = (room_t *) malloc(sizeof(room_t));
  new_room->next = room;
  room = new_room;

  room->room_id = room_id;
  room->host = host;
  room->guest = guest;
  room->vhost = vhost;

  room_cnt++;
  return room;
}

room_t *get_room_by_id(int room_id) {
  for (room_t *ptr = room; ptr != NULL; ptr = ptr->next) {
    if (ptr->room_id == room_id) {
      return ptr;
    }
  }
  return NULL;
}

void remove_room_by_id(int room_id) {
  if (room->room_id == room_id) {
    room_t *tmp = room;
    room = room->next;
    free(tmp);
    room_cnt--;
  } else {
    for (room_t *ptr = room; ptr != NULL && ptr->next != NULL; ptr = ptr->next) {
      if (ptr->next->room_id == room_id) {
        room_t *new_next = ptr->next->next;
        free(ptr->next);
        room_cnt--;
        ptr->next = new_next;
      }
    }
  }
}

room_t *get_room() {
  return room;
}

int get_room_cnt() {
  return room_cnt;
}