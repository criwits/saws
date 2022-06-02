/**
 * @file entities.h
 * @brief 飞机
 */
#ifndef SAWS_AIRCRAFT_H
#define SAWS_AIRCRAFT_H

typedef struct aircraft_s {
  // 使用双向链表提升性能
  struct aircraft_s *next;
  struct aircraft_s *prev;
  int id;
  int hp;
} aircraft_t;

#endif //SAWS_AIRCRAFT_H
