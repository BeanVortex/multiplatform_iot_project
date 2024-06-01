use this on websocket library. there is a conflict between Hash.h of esp8266 and Hash.h of Crypto library
copy this to the websocket lib path
adjust WebSockets.cpp and include HashUtil.h