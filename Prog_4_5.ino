#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <SPI.h>
#include <SD.h>
File myFile;
Stepper myStepper(200, 6, 7, 8, 9);
LiquidCrystal_I2C lcd(0x27, 20, 4);
byte up[] = { 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04 };
byte down[] = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F, 0x0E, 0x04 };
byte prev[] = { 0x00, 0x00, 0x04, 0x0C, 0x1F, 0x0C, 0x04, 0x00 };
byte next[] = { 0x00, 0x00, 0x04, 0x06, 0x1F, 0x06, 0x04, 0x00 };
byte selc[] = { 0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00, 0x00 };
bool m_cw = false;
bool m_ccw = false;
bool ada_error = false;
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
int d = 0;
int e = 0;
int f = 0;
int h = 0;
int j = 0;
int k = 0;
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, up);
  lcd.createChar(1, down);
  lcd.createChar(2, prev);
  lcd.createChar(3, next);
  lcd.createChar(4, selc);
  lcd.setCursor(0, 0);
  lcd.print(F("Simulasi Syrnge Pump"));
  lcd.setCursor(8, 1);
  lcd.write(0);
  lcd.print(F(" S1"));
  lcd.setCursor(3, 2);
  lcd.write(2);
  lcd.print(F(" S3"));
  lcd.setCursor(13, 2);
  lcd.write(3);
  lcd.print(F(" S4"));
  lcd.setCursor(8, 3);
  lcd.write(1);
  lcd.print(F(" S2"));
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println(F("initialization failed!"));
    while (1)
      ;
  }
  delay(2000);
}
void loop() {
  int s1 = !digitalRead(2);  //up
  int s2 = !digitalRead(3);  //down
  int s3 = !digitalRead(4);  //prev
  int s4 = !digitalRead(5);  //next
  int ls1 = analogRead(A6);
  int ls2 = analogRead(A7);
  if (s1 != s1a && s1) {
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
  s1a = s1;
  if (s2 != s2a && s2) {
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
  s2a = s2;
  if (s3 != s3a && s3) {
    if (hal < 4) hal--;
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
      case 4:
        hal4();
        break;
    }
    cek();
  }
  s3a = s3;
  if (ls1 == 0 & ls2 == 0) h = 1;
  if (ls1 > 0 & ls2 > 0) h = 2;
  if (ls1 == 0 & ls2 > 0) h = 3;
  if (ls1 > 0 & ls2 == 0) h = 4;
  if (s4 != s4a && s4) {
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
  s4a = s4;
  if ((m_cw || m_ccw) && millis() - skr > 100) {
    if (m_cw) myStepper.step(1);
    if (m_ccw) myStepper.step(-1);
    e = e + d;
    f = f + 6;
    float g = f / 3600.0;
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
    if (e < 10) lcd.print(F(" "));
    if (e < 100) lcd.print(F(" "));
    if (e < 1000) lcd.print(F(" "));
    if (e < 10000) lcd.print(F(" "));
    lcd.print(e);
    lcd.setCursor(12, 2);
    lcd.print(g, 3);
    skr = millis();
  }
  if (ada_error && millis() - skr > 100) {
    j++;
    digitalWrite(A0, j % 2);
    digitalWrite(A1, j % 2);
    digitalWrite(A2, j % 2);
    digitalWrite(A3, j % 2);
    skr = millis();
  }
}
void hal0() {
  lcd.setCursor(0, 0);
  lcd.print(F("1. Ukuran Syringe:  "));
  lcd.setCursor(0, 1);
  lcd.print(F("  a. 5mL            "));
  lcd.setCursor(0, 2);
  lcd.print(F("  b. 10mL           "));
  lcd.setCursor(0, 3);
  lcd.print(F("  c. 20mL           "));
  lcd.setCursor(18, pil1);
  lcd.write(4);
  digitalWrite(A0, HIGH);  //tahap1
}
void hal1() {
  lcd.setCursor(0, 0);
  lcd.print(F("2. Target Volume:   "));
  lcd.setCursor(0, 1);
  lcd.print(F("  a. 5mL            "));
  lcd.setCursor(0, 2);
  lcd.print(F("  b. 20mL           "));
  lcd.setCursor(0, 3);
  lcd.print(F("  c. 40mL           "));
  lcd.setCursor(18, pil2);
  lcd.write(4);
}
void hal2() {
  lcd.setCursor(0, 0);
  lcd.print(F("3. Laju Aliran:     "));
  lcd.setCursor(0, 1);
  lcd.print(F("  a. 5mL/jam        "));
  lcd.setCursor(0, 2);
  lcd.print(F("  b. 10mL/jam       "));
  lcd.setCursor(0, 3);
  lcd.print(F("  c. 20mL/jam       "));
  lcd.setCursor(18, pil3);
  lcd.write(4);
}
void hal3() {
  lcd.setCursor(0, 0);
  lcd.print(F("Ukuran Syringe:   mL"));
  lcd.setCursor(16, 0);
  if (pil1 == 1) {
    lcd.print(F(" 5"));
    k = 5;
  }
  if (pil1 == 2) {
    lcd.print(F("10"));
    k = 10;
  }
  if (pil1 == 3) {
    lcd.print(F("20"));
    k = 20;
  }
  lcd.setCursor(0, 1);
  lcd.print(F("Target Volume:  40mL"));
  lcd.setCursor(16, 1);
  if (pil2 == 1) {
    lcd.print(F(" 5"));
    a = 5;
  }
  if (pil2 == 2) {
    lcd.print(F("20"));
    a = 20;
  }
  if (pil2 == 3) {
    lcd.print(F("40"));
    a = 40;
  }
  lcd.setCursor(0, 2);
  lcd.print(F("Laju Aliran:  mL/jam"));
  lcd.setCursor(12, 2);
  if (pil3 == 1) {
    lcd.print(F(" 5"));
    b = 5;
  }
  if (pil3 == 2) {
    lcd.print(F("10"));
    b = 10;
  }
  if (pil3 == 3) {
    lcd.print(F("20"));
    b = 20;
  }
  lcd.setCursor(0, 3);
  lcd.print(F("Target Waktu:    jam"));
  c = a / (b * 1.0);
  d = round(25000 / (c * 600));
  e = 0;
  lcd.setCursor(13, 3);
  lcd.print(c, 2);
  digitalWrite(A1, HIGH);  //tahap2
  digitalWrite(A0, LOW);
  catat(k, a, b, c);
}
void hal4() {
  lcd.setCursor(0, 0);
  lcd.print(F("Target Langkah:25000"));
  if (h == 1) {
    lcd.setCursor(0, 1);
    lcd.print(F("Jumlah Langkah:     "));
    lcd.setCursor(0, 2);
    lcd.print(F("Waktu Lewat:     jam"));
    ada_error = false;
    m_cw = true;
    digitalWrite(A2, HIGH);  //tahap3
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A3, LOW);
  } else if (h > 2) {
    lcd.setCursor(0, 1);
    lcd.print(F("LS1/LS2 belum dtutup"));
    lcd.setCursor(0, 2);
    lcd.print(F("Periksa lagi LS1/LS2"));
    ada_error = true;
  } else {
    lcd.setCursor(0, 1);
    lcd.print(F("Tutup LS1 dan LS2   "));
    lcd.setCursor(0, 2);
    lcd.print(F("agar Motr Stepper ON"));
    e = 0;
    f = 0;
  }
}
void hal5() {
  lcd.setCursor(0, 0);
  lcd.print(F("Kembali ke pos. awal"));
  lcd.setCursor(0, 3);
  lcd.print(F("Tekan S4 utk berikut"));
  if (h == 2) {
    lcd.setCursor(0, 1);
    lcd.print(F("Jumlah Langkah:     "));
    lcd.setCursor(0, 2);
    lcd.print(F("Waktu Lewat:     jam"));
    m_ccw = true;
    digitalWrite(A3, HIGH);  //tahap4
    digitalWrite(A2, LOW);
  } else {
    lcd.setCursor(0, 1);
    lcd.print(F("Buka LS1 dan LS2    "));
    lcd.setCursor(0, 2);
    lcd.print(F("agar Stepper kembali"));
    e = 0;
    f = 0;
  }
}
void hal6() {
  lcd.setCursor(0, 0);
  lcd.print(F("Proses telah selesai"));
  lcd.setCursor(0, 1);
  lcd.print(F("Tekan tombol S3 / S4"));
  lcd.setCursor(0, 2);
  lcd.print(F("untuk memulai lagi  "));
  lcd.setCursor(0, 3);
  lcd.print(F("tahapan dari awal   "));
  digitalWrite(A3, LOW);  //selesai
  tampil();
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
  Serial.print(c);
  Serial.print(',');
  Serial.print(d);
  Serial.print(',');
  Serial.println(e);
}
void catat(int a, int b, int c, float d) {
  myFile = SD.open("riwayat.txt", FILE_WRITE);
  if (myFile) {
    myFile.print("us:");
    myFile.print(a);
    myFile.print(", tv:");
    myFile.print(b);
    myFile.print(", la:");
    myFile.print(c);
    myFile.print(", dw:");
    myFile.println(d);
    myFile.close();
  }
}
void tampil() {
  myFile = SD.open("riwayat.txt");
  if (myFile) {
    Serial.println(F("Riwayat penggunaan:"));
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  }
}