#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
byte up[] = { 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04 };
byte down[] = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F, 0x0E, 0x04 };
byte prev[] = { 0x00, 0x00, 0x04, 0x0C, 0x1F, 0x0C, 0x04, 0x00 };
byte next[] = { 0x00, 0x00, 0x04, 0x06, 0x1F, 0x06, 0x04, 0x00 };
byte select[] = { 0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00, 0x00 };
int hal = -1;
int pil1 = 1;
int pil2 = 1;
int pil3 = 1;
int s1a = 0;
int s2a = 0;
int s3a = 0;
int s4a = 0;
int a = 0;
int b = 0;
float c = 0.0;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, up);
  lcd.createChar(1, down);
  lcd.createChar(2, prev);
  lcd.createChar(3, next);
  lcd.createChar(4, select);
  lcd.setCursor(0, 0);
  lcd.print("Simulasi Syrnge Pump");
  lcd.setCursor(8, 1);
  lcd.write(0);
  lcd.print(" S1");
  lcd.setCursor(3, 2);
  lcd.write(2);
  lcd.print(" S3");
  lcd.setCursor(13, 2);
  lcd.write(3);
  lcd.print(" S4");
  lcd.setCursor(8, 3);
  lcd.write(1);
  lcd.print(" S2");
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  delay(2000);
}
void loop() {
  int s1 = !digitalRead(2);  //up
  int s2 = !digitalRead(3);  //down
  int s3 = !digitalRead(4);  //prev
  int s4 = !digitalRead(5);  //next
  if (s1 != s1a && s1) {
    if (hal == 0) {
      pil1--;
      if (pil1 < 1) pil1 = 1;
      hal0();
      cek();
    }
    if (hal == 1) {
      pil2--;
      if (pil2 < 1) pil2 = 1;
      hal1();
      cek();
    }
    if (hal == 2) {
      pil3--;
      if (pil3 < 1) pil3 = 1;
      hal2();
      cek();
    }
  }
  s1a = s1;
  if (s2 != s2a && s2) {
    if (hal == 0) {
      pil1++;
      if (pil1 > 3) pil1 = 3;
      hal0();
      cek();
    }
    if (hal == 1) {
      pil2++;
      if (pil2 > 3) pil2 = 3;
      hal1();
      cek();
    }
    if (hal == 2) {
      pil3++;
      if (pil3 > 3) pil3 = 3;
      hal2();
      cek();
    }
  }
  s2a = s2;
  if (s3 != s3a && s3) {
    hal--;
    if (hal < 0) hal = 0;
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
  s3a = s3;
  if (s4 != s4a && s4) {
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
  s4a = s4;
}
void hal0() {
  lcd.setCursor(0, 0);
  lcd.print("1. Ukuran Syringe:  ");
  lcd.setCursor(0, 1);
  lcd.print("  a. 5mL            ");
  lcd.setCursor(0, 2);
  lcd.print("  b. 10mL           ");
  lcd.setCursor(0, 3);
  lcd.print("  c. 20mL           ");
  lcd.setCursor(18, pil1);
  lcd.write(4);
}
void hal1() {
  lcd.setCursor(0, 0);
  lcd.print("2. Target Volume:   ");
  lcd.setCursor(0, 1);
  lcd.print("  a. 5mL            ");
  lcd.setCursor(0, 2);
  lcd.print("  b. 20mL           ");
  lcd.setCursor(0, 3);
  lcd.print("  c. 40mL           ");
  lcd.setCursor(18, pil2);
  lcd.write(4);
}
void hal2() {
  lcd.setCursor(0, 0);
  lcd.print("3. Laju Aliran:     ");
  lcd.setCursor(0, 1);
  lcd.print("  a. 5mL/jam        ");
  lcd.setCursor(0, 2);
  lcd.print("  b. 10mL/jam       ");
  lcd.setCursor(0, 3);
  lcd.print("  c. 20mL/jam       ");
  lcd.setCursor(18, pil3);
  lcd.write(4);
}
void hal3() {
  lcd.setCursor(0, 0);
  lcd.print("Ukuran Syringe:   mL");
  lcd.setCursor(16, 0);
  if (pil1 == 1) lcd.print(" 5");
  if (pil1 == 2) lcd.print("10");
  if (pil1 == 3) lcd.print("20");
  lcd.setCursor(0, 1);
  lcd.print("Target Volume:  40mL");
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
  lcd.print("Laju Aliran:  mL/jam");
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
  lcd.print("Durasi waktu:    jam");
  c = a / b;
  lcd.setCursor(14, 3);
  lcd.print(c, 1);
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