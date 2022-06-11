/**
 * @file prop.c
 * @brief 道具
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <game/room.h>

void add_prop(int id, int kind, room_t *room) {
  room->prop_cnt++;
  prop_t *new_prop = (prop_t *) malloc(sizeof(prop_t));
  new_prop->prev = NULL;
  new_prop->next = room->prop_list;
  new_prop->id = id;
  new_prop->kind = kind;
  if (room->prop_list != NULL) {
    room->prop_list->prev = new_prop;
  }
  room->prop_list = new_prop;
}

void remove_given_prop(prop_t *ptr, room_t *room) {
  room->prop_cnt--;
  if (ptr->prev == NULL && ptr->next == NULL) {
    room->prop_list = NULL;
  } else if (ptr->prev == NULL) {
    ptr->next->prev = NULL;
    room->prop_list = room->prop_list->next;
  } else if (ptr->next == NULL) {
    ptr->prev->next = NULL;
  } else {
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
  }
  free(ptr);
}

void remove_prop(int id, room_t *room) {
  for (prop_t *ptr = room->prop_list; ptr != NULL;) {
    if (ptr->id == id) {
      prop_t *next = ptr->next;
      remove_given_prop(ptr, room);
      ptr = next;
    } else {
      ptr = ptr->next;
    }
  }
}

prop_t *get_prop(int id, room_t *room) {
  for (prop_t *ptr = room->prop_list; ptr != NULL; ptr = ptr->next) {
    if (ptr->id == id) {
      return ptr;
    }
  }
  return NULL;
}

void remove_all_props(room_t *room) {
  for (prop_t *ptr = room->prop_list; ptr != NULL;) {
    prop_t *next = ptr->next;
    free(ptr);
    ptr = next;
  }
  room->prop_list = NULL;
  room->prop_cnt = 0;
}