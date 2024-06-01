#include "WebsocketUtil.h"

const char *serverAddress = "192.168.1.105";
const char *serverUrl = "/chat";
const int serverPort = 8080;
WebSocketsClient webSocket;

const int buzz = 5;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    String message = (char *)payload;
    if (message.startsWith("{e}"))
        message = decryptAES(message.substring(3).c_str());

    if (message.equals("Fire"))
        return;

    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        webSocket.sendTXT("subscribe");
        break;
    case WStype_TEXT:
        Serial.print("[WSc] get text: \n");
        Serial.println(message);
        if (message.equals("login success"))
            playLoginSuccessTone();

        if (message.equals("answer_result=Correct answer"))
            playCorrectAnswerTone();

        if (message.equals("login failed") || message.equals("answer_result=Wrong answer"))
            playFailedTone();

        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);
        break;
    case WStype_ERROR:
        Serial.printf("[WSc] Error!\n");
        break;
    default:
        Serial.printf("[WSc] Unknown event type: %u\n", type);
    }
}

void initWebsocket()
{
    webSocket.begin(serverAddress, serverPort, serverUrl);

    webSocket.onEvent(webSocketEvent);

    // try ever 5000 again if connection has failed
    webSocket.setReconnectInterval(3000);
}

void websocketLoop()
{
    webSocket.loop();
}

void websocketSendTXT(String msg)
{
    String encrypted = "{e}" + encryptAES(msg.c_str());
    webSocket.sendTXT(msg);
}

void playLoginSuccessTone()
{
    int melody[] = {262, 294, 330, 349, 392};
    int noteDurations[] = {200, 200, 200, 200, 400};
    for (int thisNote = 0; thisNote < 5; thisNote++)
    {
        int noteDuration = noteDurations[thisNote];
        tone(buzz, melody[thisNote], noteDuration);
        delay(noteDuration * 1.30);
    }
    noTone(buzz);
}

void playCorrectAnswerTone()
{
    int melody[] = {440, 494, 523};
    int noteDurations[] = {200, 200, 400};
    for (int thisNote = 0; thisNote < 3; thisNote++)
    {
        int noteDuration = noteDurations[thisNote];
        tone(buzz, melody[thisNote], noteDuration);
        delay(noteDuration * 1.30);
    }
    noTone(buzz);
}

void playFailedTone()
{
    int melody[] = {523, 494, 440};
    int noteDurations[] = {200, 200, 400};
    for (int thisNote = 0; thisNote < 3; thisNote++)
    {
        int noteDuration = noteDurations[thisNote];
        tone(buzz, melody[thisNote], noteDuration);
        delay(noteDuration * 1.30);
    }
    noTone(buzz);
}