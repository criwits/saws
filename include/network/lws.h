#ifndef SAWS_LWS_H
#define SAWS_LWS_H

void lws_init(int port);
void lws_stop();
int lws_loop();

#endif //SAWS_LWS_H
