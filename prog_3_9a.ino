#include <PID_v2.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include "GyverMAX6675.h"
#include <SPI.h>
#include <SD.h>
File myFile;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
#define DT 4
#define CK 5
#define SS 6
GyverMAX6675<CK, DT, SS> termosensor;
int detik = 0;
int dur_dt = -1;
int dur_mn = -1;
float tmk = 0;
float vout = 0;
float pres = 0;
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;
PID_v2 myPID(consKp, consKi, consKd, PID::Direct);
unsigned long skr = 0;
int spsuhu1 = 496;  //121 C
int spsuhu2 = 549;  //134 C
int spsuhu1_c = 121;
int spsuhu2_c = 134;
int sptek1 = 103;  //kPa
int sptek2 = 206;  //kPa
int spsuhu = 0;
int sptek = 0;
int st = 0;
int s2a = 0;
int selisih[10];
int a = 0;
int ada_error = 0;
int error_dulu = 0;
int tahap = 0;
bool lihatfile = false;
int rerata = 100;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 2);
  lcd.print("Tutup S2 untuk mulai");
  analogReference(EXTERNAL);  // AREF = 2.5V
  pinMode(2, INPUT_PULLUP);   //S1
  pinMode(3, INPUT_PULLUP);   //S2
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetSampleTime(10);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  delay(1000);
}
void loop() {
  if (lihatfile) {
    myFile = SD.open("rekam.txt");
    if (myFile) {
      Serial.println("file rekam.txt:");
      while (myFile.available()) {
        Serial.write(myFile.read());
      }
      myFile.close();
    } else Serial.println("tidak ada file");
    lihatfile = false;
  } else {
    DateTime now = rtc.now();
    if (now.second() != detik && ada_error == 0) {
      detik = now.second();
      lcd.setCursor(0, 0);
      lcd.print(now.year(), DEC);
      lcd.print('/');
      lcd.print(now.month(), DEC);
      lcd.print('/');
      lcd.print(now.day(), DEC);
      lcd.print(" ");
      if (now.hour() < 10) lcd.print("0");
      lcd.print(now.hour(), DEC);
      lcd.print(':');
      if (now.minute() < 10) lcd.print("0");
      lcd.print(now.minute(), DEC);
      lcd.print(':');
      if (now.second() < 10) lcd.print("0");
      lcd.print(now.second(), DEC);
      lcd.print(" ");
    }
    int s1 = !digitalRead(2);
    if (s1) {
      spsuhu = spsuhu2;
      sptek = sptek2;
      st = spsuhu2_c;
    } else {
      spsuhu = spsuhu1;
      sptek = sptek1;
      st = spsuhu1_c;
    }
    int s2 = !digitalRead(3);
    if (s2 != s2a) {
      if (s2) {
        myPID.Start(analogRead(A7), 0, spsuhu);
        tahap = 1;
      } else {
        myPID.Start(analogRead(A7), 0, 0);
      }
    }
    s2a = s2;
    double sensor = analogRead(A7);
    double gap = abs(myPID.GetSetpoint() - sensor);
    if (gap < 10) {
      myPID.SetTunings(consKp, consKi, consKd);
    } else {
      myPID.SetTunings(aggKp, aggKi, aggKd);
    }
    double output = myPID.Run(sensor);
    analogWrite(9, abs(output));
    if (output > 0) {
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
    }
    if (output < 0) {
      digitalWrite(8, LOW);
      digitalWrite(7, HIGH);
    }
    if (millis() - skr > 1000) {
      if (termosensor.readTemp()) tmk = termosensor.getTemp();
      vout = analogRead(A6) * 2.5 / 1024.0;
      pres = vout / (2.5 * 0.004) + 3;
      lcd.setCursor(0, 3);
      lcd.print("T: ");
      if (tmk < 10) lcd.print(" ");
      if (tmk < 100) lcd.print(" ");
      lcd.print(tmk);
      lcd.setCursor(9, 3);
      lcd.print("| P: ");
      if (pres < 10) lcd.print(" ");
      if (pres < 100) lcd.print(" ");
      lcd.print(pres);
      selisih[a] = abs(spsuhu - sensor);
      a++;
      if (a > 9) {
        a = 0;
        rerata = 0;
        for (int i = 0; i < 10; i++) {
          rerata += selisih[i];
        }
      }
      double pv = sensor * 0.244141;
      Serial.print(st);
      Serial.print(',');
      Serial.print(pv);
      Serial.print(',');
      Serial.print(tmk);
      Serial.print(',');
      Serial.print(pres);
      Serial.print(',');
      Serial.print(ada_error);
      Serial.print(',');
      Serial.println(tahap);
      lcd.setCursor(0, 1);
      lcd.print("SP:      | PV:      ");
      lcd.setCursor(3, 1);
      lcd.print(st);
      lcd.setCursor(14, 1);
      lcd.print(pv);
      if (tahap == 3 && pv < 80) {
        myPID.Start(analogRead(A7), 0, 160);  //160 = 39C
        tahap = 4;
      }
      if (tahap == 4 && pv < 40) {
        myPID.Start(analogRead(A7), 0, 0);  //0 = 0C
        tahap = 5;
      }
      if (pres > sptek && tmk > st) {
        lcd.setCursor(0, 0);
        lcd.print("Suhu&Tek lampaui bts");
        ada_error = 1;
      } else if (pres > sptek) {
        lcd.setCursor(0, 0);
        lcd.print("Tekanan mlampaui bts");
        ada_error = 2;
      } else if (tmk > st) {
        lcd.setCursor(0, 0);
        lcd.print("Suhu melampaui batas");
        ada_error = 3;
      } else {
        ada_error = 0;
      }
      if (ada_error > 0) {
        digitalWrite(A0, a % 2);
        digitalWrite(A1, a % 2);
        digitalWrite(A2, a % 2);
        digitalWrite(A3, a % 2);
        if (ada_error != error_dulu) {
          error_dulu = ada_error;
          rekam(now.hour(), now.minute(), now.second(), ada_error, s1, pres, tmk);
        }
      }
      skr = millis();
    }
    if (rerata < 100 && tahap == 1) {
      lcd.setCursor(0, 2);
      if (now.hour() < 10) lcd.print("0");
      lcd.print(now.hour(), DEC);
      lcd.print(':');
      if (now.minute() < 10) lcd.print("0");
      lcd.print(now.minute(), DEC);
      lcd.print(':');
      if (now.second() < 10) lcd.print("0");
      lcd.print(now.second(), DEC);
      lcd.print(" - Timer ON ");
      dur_dt = now.second();
      if (s1) dur_mn = 1 + now.minute();  //simulasi 2 menit
      else dur_mn = 2 + now.minute();     //simulasi 1 menit
      if (dur_mn > 60) dur_mn = dur_mn - 60;
      tahap = 2;
    }
    if (dur_dt == now.second() && dur_mn == now.minute() && tahap == 2) {
      lcd.setCursor(11, 2);
      if (now.hour() < 10) lcd.print("0");
      lcd.print(now.hour(), DEC);
      lcd.print(':');
      if (now.minute() < 10) lcd.print("0");
      lcd.print(now.minute(), DEC);
      lcd.print(':');
      if (now.second() < 10) lcd.print("0");
      lcd.print(now.second(), DEC);
      dur_mn = -1;
      tahap = 3;
      myPID.Start(analogRead(A7), 0, 324);  //324 = 79C
    }
    if (tahap == 1 && !ada_error) {
      digitalWrite(A0, HIGH);
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      lcd.setCursor(0, 2);
      lcd.print("Tahap1 : Pemanasan  ");
    }
    if (tahap == 2 && !ada_error) {
      digitalWrite(A0, LOW);
      digitalWrite(A1, HIGH);
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
    }
    if (tahap == 3 && !ada_error) {
      digitalWrite(A0, LOW);
      digitalWrite(A1, LOW);
      digitalWrite(A2, HIGH);
      digitalWrite(A3, LOW);
    }
    if (tahap == 4 && !ada_error) {
      digitalWrite(A0, LOW);
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A3, HIGH);
      lcd.setCursor(0, 2);
      lcd.print("Tahap4 : Pendinginan");
    }
    if (tahap == 5 && !ada_error) {
      digitalWrite(A0, LOW);
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      lcd.setCursor(0, 2);
      lcd.print("Sterilisasi selesai ");
    }
  }
}
void rekam(int a, int b, int c, int d, int e, int f, int g) {
  myFile = SD.open("rekam.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(a);
    myFile.print(":");
    myFile.print(b);
    myFile.print(":");
    myFile.print(c);
    myFile.print(",");
    if (d == 1) myFile.print("suhu&tek tinggi, mode:");
    if (d == 2) myFile.print("tekanan tinggi, mode:");
    if (d == 3) myFile.print("suhu tinggi, mode:");
    if (e == 1) myFile.print("ekstrem, tekanan:");
    else myFile.print("standar, tekanan:");
    myFile.print(f);
    myFile.print(", suhu:");
    myFile.println(g);
    myFile.close();
  }
}
void serialEvent() {
  while (Serial.available()) {
    int data = Serial.parseInt();
    if (Serial.read() == char(13)) {
      if (data == 1) {
        Serial.println("oven dimatikan");
        myPID.Start(analogRead(A7), 0, 0);
      }
      if (data == 2) {
        Serial.println("lihat file:");
        lihatfile = true;
      }
    }
  }
}