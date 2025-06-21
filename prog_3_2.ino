#include <PID_v2.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
int detik = 0;
int durasi_detik = -1;
int durasi_menit = -1;
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;
PID_v2 myPID(consKp, consKi, consKd, PID::Direct);
unsigned long skr = 0;
int setpoin1 = 496;  //121 C
int setpoin2 = 549;  //134 C
int setpoin = 0;
int s1a = 0;
int selisih[10];
int a = 0;
bool b = false;
int rerata = 100;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  analogReference(EXTERNAL);  // AREF = 2.5V
  pinMode(2, INPUT_PULLUP);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
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
  if (s1 != s1a || setpoin == 0) {
    if (s1) setpoin = setpoin2;
    else setpoin = setpoin1;
    myPID.Start(analogRead(A7), 0, setpoin);
  }
  s1a = s1;
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
    skr = millis();
  }
  if (rerata < 100 && !b) {
    lcd.setCursor(0, 2);
    lcd.print("Timer    menit : ON ");
    lcd.setCursor(6, 2);
    if (s1) lcd.print(" 5");
    else lcd.print("20");
    lcd.setCursor(0, 3);
    if (now.hour() < 10) lcd.print("0");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    if (now.second() < 10) lcd.print("0");
    lcd.print(now.second(), DEC);
    durasi_detik = now.second();
    if (s1) durasi_menit = 1 + now.minute();  //simulasi 2 menit
    else durasi_menit = 2 + now.minute();     //simulasi 1 menit
    if (durasi_menit > 60) durasi_menit = durasi_menit - 60;
    b = true;
  }
  if (durasi_detik == now.second() && durasi_menit == now.minute()) {
    lcd.setCursor(17, 2);
    lcd.print("OFF");
    lcd.setCursor(9, 3);
    lcd.print("- ");
    if (now.hour() < 10) lcd.print("0");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    if (now.second() < 10) lcd.print("0");
    lcd.print(now.second(), DEC);
    durasi_menit = -1;
    myPID.Start(analogRead(A7), 0, 0);//mematikan oven
  }
}