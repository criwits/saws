/**
 * @file difficulty.c
 * @brief 难度设置器
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <game/room.h>
#include <game/logic.h>

void set_difficulty(room_t *room, int difficulty) {
  switch (difficulty) {
    case 0:
      room->has_boss = false;
      room->boss_hp_increase = 0;
      room->enemy_max_number = 5;
      room->enemy_bullet_power = 20;
      room->boss_bullet_power = 40;
      room->enemy_speed_y_increase = 0;
      room->enemy_speed_y = 5;
      room->enemy_speed_y_max = 5;
      break;
    case 1:
      room->has_boss = true;
      room->boss_hp_increase = 0;
      room->enemy_max_number = 8;
      room->enemy_bullet_power = 30;
      room->boss_bullet_power = 45;
      room->enemy_speed_y_increase = 0;
      room->enemy_speed_y = 6;
      room->enemy_speed_y_max = 6;
      break;
    case 2:
      room->has_boss = true;
      room->boss_hp_increase = 20;
      room->enemy_max_number = 10;
      room->enemy_bullet_power = 35;
      room->boss_bullet_power = 50;
      room->enemy_speed_y_increase = 1;
      room->enemy_speed_y = 6;
      room->enemy_speed_y_max = 10;
    default:
      assert(0);
      break;
  }
}