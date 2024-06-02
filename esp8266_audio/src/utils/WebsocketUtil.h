#ifndef WEBSOCKETUTIL_H
#define WEBSOCKETUTIL_H

#include <Arduino.h>
#include <WebSocketsClient.h>
#include "AESUtil.h"
#include "DYPlayerArduino.h"


void initWebsocket();
void websocketLoop();
void websocketSendTXT(String msg);
void playLoginSuccessTone();
void playCorrectAnswerTone();
void playFailedTone();

#endif
