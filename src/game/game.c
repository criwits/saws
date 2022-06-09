/**
 * @file game.c
 * @brief 游戏主逻辑
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <game/room.h>
#include <timer/timer.h>
#include <time.h>
#include <game/msg.h>
#include <network/send.h>
#include <sys/time.h>

void create_npc(room_t *room, int call_cnt) {
  if (room->npc_cnt < room->enemy_max_number) {
    // 可以生成 NPC
    saws_debug("Spawn NPC for room id %d", room->room_id);
    struct timeval tval;
    gettimeofday( &tval, NULL );
    srand((unsigned) tval.tv_usec);
    struct npc_spawn_s msg = {
        .id = room->npc_id++,
        .location_y = rand() % (int)(512 * room->ratio),
        .location_x = rand() % 512,
        .speed_y = room->enemy_speed_y
    };
    if (rand() % 10 <3) {
      msg.speed_x = rand() % 2 + 1;
      msg.hp = 20;
      msg.mob = 1;
    } else {
      msg.speed_x = 0;
      msg.hp = 10;
      msg.mob = 0;
    }
    room->npc_cnt++;
    send_message(room->vhost, room->host->wsi, &msg, NPC_SPAWN);
    send_message(room->vhost, room->guest->wsi, &msg, NPC_SPAWN);
  }
}

void start_game(room_t *room) {
  saws_debug("Room %d starts", room->room_id);
  room->running = true;
  room->timer = start_timer(room, 800, create_npc);

}
