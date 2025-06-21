#include <PID_v2.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
int detik = 0;
int dur_dt = -1;
int dur_mn = -1;
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;
PID_v2 myPID(consKp, consKi, consKd, PID::Direct);
unsigned long skr = 0;
int setpoin1 = 496;  //121 C
int setpoin2 = 549;  //134 C
int setpoin = 0;
int s2a = 0;
int selisih[10];
int a = 0;
bool tahap1 = false;
bool tahap2 = false;
bool tahap3 = false;
bool tahap4 = false;
bool selesai = false;
int rerata = 100;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 2);
  lcd.print("Tutup S2 untuk mulai");
  lcd.setCursor(0, 3);
  lcd.print("S1 buka=121 ttup=134");
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
  delay(1000);
}
void loop() {
  DateTime now = rtc.now();
  if (now.second() != detik) {
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
  }
  int s1 = !digitalRead(2);
  if (s1) setpoin = setpoin2;
  else setpoin = setpoin1;
  int s2 = !digitalRead(3);
  if (s2 != s2a && s2) {
    if (s2) {
      myPID.Start(analogRead(A7), 0, setpoin);
      tahap1 = true;
      selesai = false;
    } else {
      myPID.Start(analogRead(A7), 0, 0);  //mematikan oven
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
  if (millis() - skr > 100) {
    selisih[a] = abs(setpoin - sensor);
    a++;
    if (a > 9) {
      a = 0;
      rerata = 0;
      for (int i = 0; i < 10; i++) {
        rerata += selisih[i];
      }
    }
    Serial.print("0,");
    double st = setpoin * 0.244141;
    double pv = sensor * 0.244141;
    Serial.print(st);
    Serial.print(',');
    Serial.println(pv);
    lcd.setCursor(0, 1);
    lcd.print("SP:      | PV:      ");
    lcd.setCursor(3, 1);
    lcd.print(st);
    lcd.setCursor(14, 1);
    lcd.print(pv);
    if (tahap3 && pv < 80) {
      tahap3 = false;
      tahap4 = true;
    }
    if (tahap4 && pv < 40) {
      tahap4 = false;
      selesai = true;
    }
    skr = millis();
  }
  if (rerata < 100 && tahap1) {
    lcd.setCursor(0, 3);
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
    tahap1 = false;
    tahap2 = true;
  }
  if (dur_dt == now.second() && dur_mn == now.minute() && tahap2) {
    lcd.setCursor(11, 3);
    if (now.hour() < 10) lcd.print("0");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    if (now.second() < 10) lcd.print("0");
    lcd.print(now.second(), DEC);
    dur_mn = -1;
    tahap2 = false;
    tahap3 = true;
    myPID.Start(analogRead(A7), 0, 0);  //mematikan oven
  }
  if (tahap1) {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    lcd.setCursor(0, 2);
    lcd.print("Tahap1 : Pemanasan  ");
    lcd.setCursor(0, 3);
    lcd.print("Oven dihidupkan     ");
  }
  if (tahap2) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    lcd.setCursor(0, 2);
    lcd.print("Tahap2 : Sterilisasi");
  }
  if (tahap3) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
    lcd.setCursor(0, 2);
    lcd.print("Tahap3 : Pengeringan");
  }
  if (tahap4) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, HIGH);
    lcd.setCursor(0, 2);
    lcd.print("Tahap4 : Pendinginan");
    lcd.setCursor(0, 3);
    lcd.print("buka S2=buka Autclve");
  }
  if (selesai) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    lcd.setCursor(0, 2);
    lcd.print("Sterilisasi selesai ");
    lcd.setCursor(0, 3);
    lcd.print("Silahkan ambil bahan");
  }
}