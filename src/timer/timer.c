/**
 * @file timer.c
 * @brief 线程相关
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <pthread.h>
#include <unistd.h>

#include <timer/model.h>

void *timer_thread(void *args) {
  timer_session_t *session = (timer_session_t *)args;
  int cnt = 0;
  while (true) {
    usleep(session->callback_ms * 1000);
    session->callback_func(session->timer_id, cnt++);
  }

  return NULL;
}

timer_session_t *start_timer(int timer_id, int callback_ms, void (*callback_func)(int, int)) {
  timer_session_t *session = (timer_session_t *)malloc(sizeof(timer_session_t));
  session->timer_id = timer_id;
  session->callback_func = callback_func;
  session->callback_ms = callback_ms;

  pthread_create(&(session->timer_thread), NULL, timer_thread, (void *)session);
  saws_log("Successfully registered timer with session id %d (%d ms per cycle)", timer_id, callback_ms);
  return session;
}

void stop_timer(timer_session_t *session) {
  pthread_cancel(session->timer_thread);
  free(session);
}