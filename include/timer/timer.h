#ifndef SAWS_TIMER_H
#define SAWS_TIMER_H

#include "model.h"

timer_session_t *start_timer(int session_id, int callback_ms, void (*callback_func)(int, int));
void stop_timer(timer_session_t *session);

#endif //SAWS_TIMER_H
