#include <WiFi.h>
#include "ThingSpeak.h"
#include <TM1637.h>
WiFiClient  client;
unsigned long skr = 0;
TM1637 tm;
const int Channel_ID = 2694062;                //silahkan diganti
const char * ReadAPIKey = " FTNM9A6Q83A2OQDJ"; //silahkan diganti
int ada_error = 0;
int ada_tahap = 0;
int status = 0;
void setup() {
  Serial.begin(9600);
  tm.begin(16, 17, 4);
  tm.displayClear();
  tm.setBrightness(7);
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wifi not connected");
  }
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  pinMode(19, OUTPUT);
}
void loop() {
  if (millis() - skr > 2000) {
    skr = millis();
    int error = ThingSpeak.readLongField(Channel_ID, 5, ReadAPIKey);
    status = ThingSpeak.getLastReadStatus();
    if (status == 200) ada_error = error;
    int tahap = ThingSpeak.readLongField(Channel_ID, 6, ReadAPIKey);
    status = ThingSpeak.getLastReadStatus();
    if (status == 200) ada_tahap = tahap;
    switch (ada_error) {
      case 1:
        tm.displayPChar("Err1");
        tone(19, 2093);
        break;
      case 2:
        tm.displayPChar("Err2");
        tone(19, 1480);
        break;
      case 3:
        tm.displayPChar("Err3");
        tone(19, 1047);
        break;
      default:
        digitalWrite(19, LOW);
        noTone(19);
        switch (ada_tahap) {
          case 1:
            tm.displayPChar("1---");
            break;
          case 2:
            tm.displayPChar("12--");
            break;
          case 3:
            tm.displayPChar("123-");
            tone(19, 523);
            break;
          case 4:
            tm.displayPChar("1234");
            tone(19, 262);
            break;
          default:
            tm.displayPChar("----");
        }
    }
    Serial.print("Error:");
    Serial.print(ada_error);
    Serial.print(", Tahap:");
    Serial.println(ada_tahap);
  }
}