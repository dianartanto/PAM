#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);
void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
}
void loop() {
  delay(10); // this speeds up the simulation
}
void serialEvent() {
  while (Serial.available()) {
    float data1 = Serial.parseFloat();//setpoin
    float data2 = Serial.parseFloat();//suhu oven
    float data3 = Serial.parseFloat();//sensor termokopel
    float data4 = Serial.parseFloat();//sensor tekanan
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