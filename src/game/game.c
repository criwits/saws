/**
 * @file game.c
 * @brief 游戏主逻辑
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <game/room.h>

void start_game(room_t *room) {
  saws_debug("Room %d starts", room->room_id);
  room->running = true;
}

void stop_game(room_t *room) {
  saws_debug("Room %d stops", room->room_id);
  room->running = false;
}

bool get_game_status(room_t *room) {
  return room != NULL && room->running;
}


