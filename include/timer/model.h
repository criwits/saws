#ifndef SAWS_MODEL_H
#define SAWS_MODEL_H

#include <common.h>
#include <pthread.h>
#include <game/room.h>

typedef void (*timer_callback_func_t)(room_t *room, int ms_cnt);

typedef struct timer_session_s {
  // struct timer_session_s *next;
  struct room_s *room;
  pthread_t timer_thread;
  int callback_ms;
  timer_callback_func_t callback_func;
} timer_session_t;

#endif //SAWS_MODEL_H
