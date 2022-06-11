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
  aircraft_t *new_npc = (aircraft_t *) malloc(sizeof(aircraft_t));
  new_npc->prev = NULL;
  new_npc->next = room->npc_list;
  new_npc->id = id;
  new_npc->mob = mob;
  new_npc->hp = hp;
  if (room->npc_list != NULL) {
    room->npc_list->prev = new_npc;
  }
  room->npc_list = new_npc;
}

void remove_given_npc(aircraft_t *ptr, room_t *room) {
  room->npc_cnt--;
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
}

void remove_npc(int id, room_t *room) {
  for (aircraft_t *ptr = room->npc_list; ptr != NULL;) {
    if (ptr->id == id) {
      aircraft_t *next = ptr->next;
      remove_given_npc(ptr, room);
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

int remove_and_score_all_npcs(room_t *room) {
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
      aircraft_t *next = ptr->next;
      remove_given_npc(ptr, room);
      ptr = next;
    } else {
      ptr = ptr->next;
    }
  }

  return score;
}

void remove_all_npcs(room_t *room) {
  for (aircraft_t *ptr = room->npc_list; ptr != NULL;) {
    aircraft_t *next = ptr->next;
    free(ptr);
    ptr = next;
  }
  room->npc_list = NULL;
  room->npc_cnt = 0;
}

