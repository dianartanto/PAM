#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
byte up[] = { 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04 };
byte down[] = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F, 0x0E, 0x04 };
byte next[] = { 0x00, 0x00, 0x04, 0x06, 0x1F, 0x06, 0x04, 0x00 };
byte selc[] = { 0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00, 0x00 };
#define ENC_CLK 32
#define ENC_DT 33
#define ENC_SW 25
int hal = -1;
int pil1 = 1;
int pil2 = 1;
int pil3 = 1;
int a = 0;
int b = 0;
float c = 0.0;
int lastClk = 0;
int lastSW = 0;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, up);
  lcd.createChar(1, down);
  lcd.createChar(2, next);
  lcd.createChar(3, selc);
  lcd.setCursor(0, 0);
  lcd.print("Simulasi Syrnge Pump");
  lcd.setCursor(7, 1);
  lcd.write(0);
  lcd.print("up");
  lcd.setCursor(10, 2);
  lcd.write(2);
  lcd.print("sw");
  lcd.setCursor(7, 3);
  lcd.write(1);
  lcd.print("dn");
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);
  pinMode(ENC_SW, INPUT_PULLUP);
}
void loop() {
  int newClk = !digitalRead(ENC_CLK);
  if (newClk != lastClk) {
    int dtValue = digitalRead(ENC_DT);
    if (newClk && dtValue == LOW) {
      if (hal == 0) {
        pil1++;
        if (pil1 > 3) pil1 = 3;
        hal0();
      }
      if (hal == 1) {
        pil2++;
        if (pil2 > 3) pil2 = 3;
        hal1();
      }
      if (hal == 2) {
        pil3++;
        if (pil3 > 3) pil3 = 3;
        hal2();
      }
      cek();
    }
    if (newClk && dtValue == HIGH) {
      if (hal == 0) {
        pil1--;
        if (pil1 < 1) pil1 = 1;
        hal0();
      }
      if (hal == 1) {
        pil2--;
        if (pil2 < 1) pil2 = 1;
        hal1();
      }
      if (hal == 2) {
        pil3--;
        if (pil3 < 1) pil3 = 1;
        hal2();
      }
      cek();
    }
  }
  lastClk = newClk;
  int newSW = !digitalRead(ENC_SW);
  if (newSW != lastSW && newSW) {
    hal++;
    if (hal > 3) hal = 3;
    switch (hal) {
      case 0:
        hal0();
        break;
      case 1:
        hal1();
        break;
      case 2:
        hal2();
        break;
      case 3:
        hal3();
        break;
    }
    cek();
  }
  lastSW = newSW;
}
void hal0() {
  lcd.setCursor(0, 0);
  lcd.print("1. Ukuran Syringe: ");
  lcd.setCursor(0, 1);
  lcd.print(" a. 5mL ");
  lcd.setCursor(0, 2);
  lcd.print(" b. 10mL ");
  lcd.setCursor(0, 3);
  lcd.print(" c. 20mL ");
  lcd.setCursor(18, pil1);
  lcd.write(3);
}
void hal1() {
  lcd.setCursor(0, 0);
  lcd.print("2. Target Volume: ");
  lcd.setCursor(0, 1);
  lcd.print(" a. 5mL ");
  lcd.setCursor(0, 2);
  lcd.print(" b. 20mL ");
  lcd.setCursor(0, 3);
  lcd.print(" c. 40mL ");
  lcd.setCursor(18, pil2);
  lcd.write(3);
}
void hal2() {
  lcd.setCursor(0, 0);
  lcd.print("3. Laju Aliran: ");
  lcd.setCursor(0, 1);
  lcd.print(" a. 5mL/jam ");
  lcd.setCursor(0, 2);
  lcd.print(" b. 10mL/jam ");
  lcd.setCursor(0, 3);
  lcd.print(" c. 20mL/jam ");
  lcd.setCursor(18, pil3);
  lcd.write(3);
}
void hal3() {
  lcd.setCursor(0, 0);
  lcd.print("Ukuran Syringe: mL");
  lcd.setCursor(16, 0);
  if (pil1 == 1) lcd.print(" 5");
  if (pil1 == 2) lcd.print("10");
  if (pil1 == 3) lcd.print("20");
  lcd.setCursor(0, 1);
  lcd.print("Target Volume: 40mL");
  lcd.setCursor(16, 1);
  if (pil2 == 1) {
    lcd.print(" 5");
    a = 5;
  }
  if (pil2 == 2) {
    lcd.print("20");
    a = 20;
  }
  if (pil2 == 3) {
    lcd.print("40");
    a = 40;
  }
  lcd.setCursor(0, 2);
  lcd.print("Laju Aliran: mL/jam");
  lcd.setCursor(12, 2);
  if (pil3 == 1) {
    lcd.print(" 5");
    b = 5;
  }
  if (pil3 == 2) {
    lcd.print("10");
    b = 10;
  }
  if (pil3 == 3) {
    lcd.print("20");
    b = 20;
  }
  lcd.setCursor(0, 3);
  lcd.print("Durasi waktu: jam");
  c = a / (b * 1.0);
  lcd.setCursor(13, 3);
  lcd.print(c, 2);
}
void cek() {
  Serial.print(hal);
  Serial.print(',');
  Serial.print(pil1);
  Serial.print(',');
  Serial.print(pil2);
  Serial.print(',');
  Serial.println(pil3);
}