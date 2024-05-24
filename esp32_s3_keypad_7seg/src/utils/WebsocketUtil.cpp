#include "WebsocketUtil.h"

const char *serverAddress = "192.168.1.105";
const char *serverUrl = "/chat";
const int serverPort = 8080;
WebSocketsClient webSocket;

int count_down_value = 0;
bool count_down_mode = false;
String count_down_value_str = "";
bool answer_mode = false;
String answer_value_str = "";
bool login_mode = false;
bool login_status = false;

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
    const uint8_t *src = (const uint8_t *)mem;
    Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
    for (uint32_t i = 0; i < len; i++)
    {
        if (i % cols == 0)
        {
            Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
        }
        Serial.printf("%02X ", *src);
        src++;
    }
    Serial.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    String message = (char *)payload;
    if (message.startsWith("{e}"))
        message = decryptAES(message.substring(3).c_str());

    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        errorBlink();
        login_status = false;
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        webSocket.sendTXT("subscribe");
        break;
    case WStype_TEXT:
        Serial.print("[WSc] get text: ");
        Serial.println(message);
        if (message.equals("Successfully subscribed"))
            successBlink();

        if (login_status)
        {
            if (message.startsWith("count_down="))
            {
                Serial.printf("got %s\n", message.substring(11, length));
                count_down_value = message.substring(11, length).toInt();
                if (count_down_value > 6000)
                    count_down_value = 5999;
            }
            if (message.startsWith("question="))
            {
                answer_mode = true;
                enableAnswerMode();
            }
        }

        if (message.equals("login success"))
        {
            login_status = true;
            successBlink();
        }

        if (message.equals("Correct answer"))
            successBlink();

        if (message.equals("login failed") || message.equals("Wrong answer"))
            errorBlink();

        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);
        break;
    case WStype_ERROR:
        Serial.printf("[WSc] Error!\n");
        errorBlink();
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

void disableModes()
{
    answer_mode = false;
    count_down_mode = false;
    login_mode = false;
    answer_value_str = "";
    count_down_value = 0;
    count_down_value_str = "";
}

void enableCountDownMode()
{
    if (!login_status)
    {
        errorBlink();
        return;
    }
    disableModes();
    count_down_mode = true;
    successBlink();
}

void enableAnswerMode()
{
    if (!login_status)
    {
        errorBlink();
        return;
    }
    if (!answer_mode)
        websocketSendTXT("new_question");

    disableModes();
    answer_mode = true;
    successBlink();
}

void enableLoginMode()
{
    if (login_status)
    {
        errorBlink();
        return;
    }
    disableModes();
    login_mode = true;
    successBlink();
}