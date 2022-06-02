#ifndef SAWS_TIMER_H
#define SAWS_TIMER_H

#include "model.h"

timer_session_t *start_timer(room_t *room, int callback_ms, void (*callback_func)(room_t *, int));
void stop_timer(timer_session_t *session);

#endif //SAWS_TIMER_H
