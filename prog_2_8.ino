#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
DHT dht(33, DHT22);
void setup() {
  Serial.begin(9600);
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1)
      ;
  }
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.display();
}
void loop() {
  float suhu = dht.readTemperature();
  float kelm = dht.readHumidity();
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("Kelm:" + String(kelm, 1) + "%");
  oled.setCursor(0, 24);
  oled.println("Suhu:" + String(suhu, 1) + "C");
  oled.display();
  if (suhu > 37) {
    digitalWrite(19, LOW);
    digitalWrite(18, HIGH);
  } else if (suhu < 35) {
    digitalWrite(19, HIGH);
    digitalWrite(18, LOW);
  } else {
    digitalWrite(19, LOW);
    digitalWrite(18, LOW);
  }
}