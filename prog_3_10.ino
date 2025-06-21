#include <WiFi.h>
#include "ThingSpeak.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TM1637.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);
WiFiClient client;
const int Channel_ID = 2645018;               //silahkan diganti
const char* writeApiKey = "B8IRM6I7MNEL2DRZ"; //silahkan diganti
const char* server = "api.thingspeak.com";
int a1 = 0;
int b1 = 0;
int ada_error = 0;
int tahap = 0;
TM1637 tm;
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
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
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(19, OUTPUT);
}
void loop() {
  int a = !digitalRead(32);//oven off
  if (a != a1 && a) {
    Serial.println("1");
  }
  a1 = a;
  int b = !digitalRead(33);//lihat file
  if (b != b1 && b) {
    Serial.println("2");
  }
  b1 = b;
  if (millis() - skr > 100) {
    skr = millis();
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
        switch (tahap) {
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
  }
}
void serialEvent() {
  while (Serial.available()) {
    int data1 = Serial.parseInt();
    float data2 = Serial.parseFloat();
    float data3 = Serial.parseFloat();
    float data4 = Serial.parseFloat();
    int data5 = Serial.parseInt();
    int data6 = Serial.parseInt();
    if (Serial.read() == char(13)) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("sp:");
      display.println(data1);
      display.print("pv:");
      display.println(data2);
      display.print("tk:");
      display.println(data3);
      display.print("pr:");
      display.println(data4);
      ada_error = data5;
      tahap = data6;
      display.display();
      ThingSpeak.setField(1, data1);
      ThingSpeak.setField(2, data2);
      ThingSpeak.setField(3, data3);
      ThingSpeak.setField(4, data4);
      ThingSpeak.setField(5, data5);
      ThingSpeak.setField(6, data6);
      int status = ThingSpeak.writeFields(Channel_ID, writeApiKey);
    }
  }
}