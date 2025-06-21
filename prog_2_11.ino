#include <WiFi.h>
#include "ThingSpeak.h"
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HX711.h>
HX711 scale;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
DHT dht(33, DHT22);
WiFiClient client;
const int Channel_ID = xxxxxxx;                //silahkan diganti
const char* writeApiKey = "xxxxxxxxxxxxxxxx";  //silahkan diganti
const char* server = "api.thingspeak.com";
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  scale.begin(26, 25);  //DT,SCK
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1)
      ;
  }
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.display();
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wifi not connected");
  }
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}
void loop() {
  if (millis() - skr > 15000) {
    float suhu = dht.readTemperature();
    float kelm = dht.readHumidity();
    uint16_t berat = round(scale.get_units() * 50 / 21);
    Serial.println(berat);
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("Kelm:" + String(kelm, 1) + "%");
    oled.setCursor(0, 24);
    oled.println("Suhu:" + String(suhu, 1) + "C");
    oled.setCursor(0, 48);
    oled.println("Bert:" + String(berat) + "g");
    oled.display();
    if (suhu > 37) {
      digitalWrite(19, LOW);
      digitalWrite(18, HIGH);
      digitalWrite(27, LOW);
      digitalWrite(14, HIGH);
      digitalWrite(12, LOW);
    } else if (suhu < 35) {
      digitalWrite(19, HIGH);
      digitalWrite(18, LOW);
      digitalWrite(27, HIGH);
      digitalWrite(14, LOW);
      digitalWrite(12, LOW);
    } else {
      digitalWrite(19, LOW);
      digitalWrite(18, LOW);
      digitalWrite(27, LOW);
      digitalWrite(14, LOW);
      digitalWrite(12, HIGH);
    }
    if (kelm > 50) noTone(13);
    else tone(13, 500);
    ThingSpeak.setField(1, suhu);
    ThingSpeak.setField(2, kelm);
    ThingSpeak.setField(3, berat);
    int status = ThingSpeak.writeFields(Channel_ID, writeApiKey);
    if (status == 200) Serial.println("sukses terkirim");
    else Serial.println("error:" + String(status));
    skr = millis();
  }
}