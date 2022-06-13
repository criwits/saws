/**
 * @file room.c
 * @brief 房间管理
 * @author hans
 * @version 0.1
 */

#include <common.h>

#include <game/logic.h>
#include <network/protocol.h>

static room_t *room = NULL;
static int room_cnt = 0;

room_t *add_room(int room_id, struct per_session_data_saws *host, struct per_session_data_saws *guest,
    struct per_vhost_data_saws *vhost) {
  room_t *new_room = (room_t *) malloc(sizeof(room_t));
  new_room->prev = NULL;
  new_room->next = room;
  if (room != NULL) {
    room->prev = new_room;
  }
  room = new_room;

  room->room_id = room_id;
  room->vhost = vhost;
  room->running = false;

  room->host = host;
  room->guest = guest;
  room->host_uid = -1;
  room->guest_uid = -1;

  room->host_height = -1;
  room->host_width = -1;
  room->guest_height = -1;
  room->guest_width = -1;

  room->npc_cnt = 0;
  room->npc_list = NULL;

  room->prop_id = 0;
  room->prop_cnt = 0;
  room->prop_list = NULL;

  room->host_score = 0;
  room->guest_score = 0;

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

void remove_given_room(room_t *ptr) {
  room_cnt--;
  if (ptr->prev == NULL && ptr->next == NULL) {
    room = NULL;
  } else if (ptr->prev == NULL) {
    ptr->next->prev = NULL;
    room = room->next;
  } else if (ptr->next == NULL) {
    ptr->prev->next = NULL;
  } else {
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
  }
  free(ptr);
}

void remove_room(int room_id) {
  room_t *target = get_room_by_id(room_id);
  if (target != NULL) {
    remove_given_room(target);
  }
}

room_t *get_room() {
  return room;
}

int get_room_cnt() {
  return room_cnt;
}