#ifndef WEBSOCKETUTIL_H
#define WEBSOCKETUTIL_H

#include <Arduino.h>
#include "AESUtil.h"
#include "BlinkUtil.h"
#include <WebSocketsClient.h>


void initWebsocket();
void websocketLoop();
void websocketSendTXT(String msg);
void enableAnswerMode();
void enableCountDownMode();
void enableLoginMode();

#endif
