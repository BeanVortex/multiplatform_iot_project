// GND          GND
// pow in       pow out(3.3)
// adc          1   tx  u
// 16   u       3   rx  u
// 14   u       5   u
// 12   u       4   u
// 13   u       2   u   (led_builtin_active_low)
// 11   nu      8   nu
// 6    nu      9   nu
// 7    nu      10  u

// df module

// tx/io0           spk+
// rx/io1           spk-
// io2              DACL
// io3              DACR
// one_line/io4     v3.3
// io5              v5  
// io6              con3
// io7              con2
// GND              con1
//        socket

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "./utils/WebsocketUtil.h"

const char *ssid = "TP-LINK_DE179F";
const char *password = "12345678";

ESP8266WiFiMulti WiFiMulti;
const int buzz = 5;

extern DY::Player player;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial.flush();

  player.begin();
  player.setVolume(20); // Set Volume to 66%
  Serial1.println("Connecting to wifi");
  Serial.println("\nConnecting to wifi");
  pinMode(buzz, OUTPUT);

  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial1.print('.');
    delay(500);
  }
  Serial1.println("\nWifi connected");
  Serial.println("Wifi connected");
  initWebsocket();
}

void playFireWarningTone()
{
  int melody[] = {784, 659, 523, 784, 659, 523};
  int noteDurations[] = {250, 250, 500, 250, 250, 500};

  for (int thisNote = 0; thisNote < 6; thisNote++)
  {
    int noteDuration = noteDurations[thisNote];
    tone(buzz, melody[thisNote], noteDuration);
    delay(noteDuration * 1.30);
  }
  noTone(buzz);
}

void loop()
{
  websocketLoop();

  if (Serial.available() > 0)
  {
    String receivedText = Serial.readStringUntil('\n');
    Serial1.print("Received: ");
    Serial1.println(receivedText);
    if (receivedText.startsWith("Fire"))
      playFireWarningTone();

    websocketSendTXT(receivedText);
  }
}
