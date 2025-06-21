#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);
int a1 = 0;
int b1 = 0;
void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
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
  } b1 = b;
}
void serialEvent() {
  while (Serial.available()) {
    float data1 = Serial.parseFloat();
    float data2 = Serial.parseFloat();
    float data3 = Serial.parseFloat();
    float data4 = Serial.parseFloat();
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
      display.display();
    }
  }
}