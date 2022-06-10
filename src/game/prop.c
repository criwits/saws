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
  if (room->prop_list == NULL) {
    room->prop_list = (prop_t *) malloc(sizeof(prop_t));
    room->prop_list->id = id;
    room->prop_list->kind = kind;
    room->prop_list->next = NULL;
    room->prop_list->prev = NULL;
  } else {
    room->prop_list->prev = (prop_t *) malloc(sizeof(prop_t));
    room->prop_list->prev->id = id;
    room->prop_list->prev->kind = kind;
    room->prop_list->prev->prev = NULL;
    room->prop_list->prev->next = room->prop_list;
    room->prop_list = room->prop_list->prev;
  }
}

void remove_prop(int id, room_t *room) {
  for (prop_t *ptr = room->prop_list; ptr != NULL;) {
    if (ptr->id == id) {
      room->prop_cnt--;
      prop_t *next = ptr->next;
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