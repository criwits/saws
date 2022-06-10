/**
 * @file logic.h
 * @brief 游戏逻辑
 * @author hans
 * @version 0.1
 */
#ifndef SAWS_LOGIC_H
#define SAWS_LOGIC_H

void set_difficulty(room_t *room, int difficulty);

void start_game(room_t *room);
void stop_game(room_t *room);
bool get_game_status(room_t *room);

void add_npc(int id, int hp, int mob, room_t *room);
void remove_npc(int id, room_t *room);
aircraft_t *get_npc(int id, room_t *room);
int remove_and_score_all_npc(room_t *room);

void add_prop(int id, int kind, room_t *room);
void remove_prop(int id, room_t *room);
prop_t *get_prop(int id, room_t *room);

#endif //SAWS_LOGIC_H
