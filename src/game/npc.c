/**
 * @file npc.c
 * @brief NPC
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <game/room.h>

void add_npc(int id, int hp, int mob, room_t *room) {
  room->npc_cnt++;
  if (mob == 2) {
    saws_debug_room("Boss generated with NPC id %d", room->room_id, id);
  }
  if (room->npc_list == NULL) {
    room->npc_list = (aircraft_t *) malloc(sizeof(aircraft_t));
    room->npc_list->id = id;
    room->npc_list->hp = hp;
    room->npc_list->mob = mob;
    room->npc_list->next = NULL;
    room->npc_list->prev = NULL;
  } else {
    room->npc_list->prev = (aircraft_t *) malloc(sizeof(aircraft_t));
    room->npc_list->prev->id = id;
    room->npc_list->prev->hp = hp;
    room->npc_list->prev->mob = mob;
    room->npc_list->prev->prev = NULL;
    room->npc_list->prev->next = room->npc_list;
    room->npc_list = room->npc_list->prev;
  }
}

void remove_npc(int id, room_t *room) {
  for (aircraft_t *ptr = room->npc_list; ptr != NULL;) {
    if (ptr->id == id) {
      room->npc_cnt--;
      aircraft_t *next = ptr->next;
      if (ptr->prev == NULL && ptr->next == NULL) {
        room->npc_list = NULL;
      } else if (ptr->prev == NULL) {
        ptr->next->prev = NULL;
        room->npc_list = room->npc_list->next;
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

aircraft_t *get_npc(int id, room_t *room) {
  for (aircraft_t *ptr = room->npc_list; ptr != NULL; ptr = ptr->next) {
    if (ptr->id == id) {
      return ptr;
    }
  }
  return NULL;
}

int remove_and_score_all_npc(room_t *room) {
  int score = 0;

  for (aircraft_t *ptr = room->npc_list; ptr != NULL;) {
    // 炸弹不影响 BOSS 机
    if (ptr->mob != 2) {
      // 加分
      switch (ptr->mob) {
        case 0:
          score += 10;
          break;
        case 1:
          score += 20;
          break;
        default:
          break;
      }

      // 删除飞机
      room->npc_cnt--;
      aircraft_t *next = ptr->next;
      if (ptr->prev == NULL && ptr->next == NULL) {
        room->npc_list = NULL;
      } else if (ptr->prev == NULL) {
        ptr->next->prev = NULL;
        room->npc_list = room->npc_list->next;
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

  return score;
}

