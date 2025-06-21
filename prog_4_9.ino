#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
byte up[] = {0x04,0x0E,0x1F,0x04,0x04,0x04,0x04,0x04};
byte down[] = {0x04,0x04,0x04,0x04,0x04,0x1F,0x0E,0x04};
byte next[] = {0x00,0x00,0x04,0x06,0x1F,0x06,0x04,0x00};
byte selc[] = {0x00,0x01,0x03,0x16,0x1C,0x08,0x00,0x00};
#define enc_clk 32
#define enc_dt  33
#define enc_sw  25
#define dir 16
#define step 17
#define ls1 14
#define ls2 12
#define led1 5
#define led2 18
#define led3 19
#define led4 23
#define buzzer 4
bool m_cw = false;
bool m_ccw = false;
bool ada_error = false;
int hal = -1;
int pil1 = 1;
int pil2 = 1;
int pil3 = 1;
int a = 0;
int b = 0;
float c = 0.0;
int d = 0;
int e = 0;
int f = 0;
float g = 0.0;
int h = 0;
int j = 0;
unsigned long skr = 0;
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
  pinMode(enc_clk, INPUT);
  pinMode(enc_dt, INPUT);
  pinMode(enc_sw, INPUT_PULLUP);
  pinMode(dir, OUTPUT);
  pinMode(step, OUTPUT);
  pinMode(ls1, INPUT_PULLUP);
  pinMode(ls2, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(buzzer, OUTPUT);
}
void loop() {
  int ls_1 = !digitalRead(ls1);
  int ls_2 = !digitalRead(ls2);
  if (ls_1 && ls_2) h = 1;
  if (!ls_1 && !ls_2 ) h = 2;
  if (!ls_1 && ls_2 ) h = 3;
  if (ls_1 && !ls_2 ) h = 4;
  int newClk = !digitalRead(enc_clk);
  if (newClk != lastClk) {
    int dtValue = digitalRead(enc_dt);
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
  int newSW = !digitalRead(enc_sw);
  if (newSW != lastSW && newSW) {
    if (hal < 4) hal++;
    if (hal == 7) hal = 0;
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
      case 4:
        hal4();
        break;
      case 5:
        hal5();
        break;
      case 6:
        hal6();
        break;
    }
    cek();
  }
  lastSW = newSW;
  if ((m_cw || m_ccw) && millis() - skr > 100) {
    if (m_cw) digitalWrite(dir, HIGH);
    if (m_ccw) digitalWrite(dir, LOW);
    digitalWrite(step, HIGH);
    digitalWrite(step, LOW);
    e = e + d;
    f = f + 6;
    g = f / 3600.0;
    if (e >= 25000) {
      if (m_cw) {
        m_cw = false;
        hal = 5;
      }
      if (m_ccw) {
        m_ccw = false;
        hal = 6;
      }
    }
    lcd.setCursor(15, 1);
    if (e < 10) lcd.print(" ");
    if (e < 100) lcd.print(" ");
    if (e < 1000) lcd.print(" ");
    if (e < 10000) lcd.print(" ");
    lcd.print(e);
    lcd.setCursor(12, 2);
    lcd.print(g, 3);
    skr = millis();
  }
  if (ada_error && millis() - skr > 100) {
    j++;
    digitalWrite(led1, j % 2);
    digitalWrite(led2, j % 2);
    digitalWrite(led3, j % 2);
    digitalWrite(led4, j % 2);
    skr = millis();
  }
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
  lcd.write(3);
  digitalWrite(led1, HIGH);  //tahap1
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
  lcd.write(3);
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
  lcd.write(3);
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
  lcd.print("Target Waktu:    jam");
  c = a / (b * 1.0);
  d = round(25000 / (c * 600));
  e = 0;
  f = 0;
  lcd.setCursor(13, 3);
  lcd.print(c, 2);
  digitalWrite(led2, HIGH);  //tahap2
  digitalWrite(led1, LOW);
}
void hal4() {
  lcd.setCursor(0, 0);
  lcd.print("Target Langkah:25000");
  if (h == 1) {
    lcd.setCursor(0, 1);
    lcd.print("Jumlah Langkah:     ");
    lcd.setCursor(0, 2);
    lcd.print("Waktu Lewat:     jam");
    ada_error = false;
    noTone(buzzer);
    m_cw = true;
    digitalWrite(led3, HIGH);  //tahap3
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led4, LOW);
  } else if (h > 2) {
    lcd.setCursor(0, 1);
    lcd.print("LS1/LS2 belum dtutup");
    lcd.setCursor(0, 2);
    lcd.print("Periksa lagi LS1/LS2");
    ada_error = true;
    tone(buzzer, 500);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Tutup LS1 dan LS2   ");
    lcd.setCursor(0, 2);
    lcd.print("agar Motr Stepper ON");
  }
}
void hal5() {
  lcd.setCursor(0, 0);
  lcd.print("Kembali ke pos. awal");
  lcd.setCursor(0, 3);
  lcd.print("Tekan SW utk berikut");
  if (h == 2) {
    lcd.setCursor(0, 1);
    lcd.print("Jumlah Langkah:     ");
    lcd.setCursor(0, 2);
    lcd.print("Waktu Lewat:     jam");
    m_ccw = true;
    digitalWrite(led4, HIGH);  //tahap4
    digitalWrite(led3, LOW);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Buka LS1 dan LS2    ");
    lcd.setCursor(0, 2);
    lcd.print("agar Stepper kembali");
    e = 0;
    f = 0;
  }
}
void hal6() {
  lcd.setCursor(0, 0);
  lcd.print("Proses telah selesai");
  lcd.setCursor(0, 1);
  lcd.print("Tekan tombol SW     ");
  lcd.setCursor(0, 2);
  lcd.print("untuk memulai lagi  ");
  lcd.setCursor(0, 3);
  lcd.print("tahapan dari awal   ");
  digitalWrite(led4, LOW);  //selesai
  hal = 7;
}
void cek() {
  Serial.print(hal);
  Serial.print(',');
  Serial.print(pil1);
  Serial.print(',');
  Serial.print(pil2);
  Serial.print(',');
  Serial.print(pil3);
  Serial.print(',');
  Serial.print(a);
  Serial.print(',');
  Serial.print(b);
  Serial.print(',');
  Serial.print(c, 1);
  Serial.print(',');
  Serial.println(d);
}