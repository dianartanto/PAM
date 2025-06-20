#include <PID_v2.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SPI.h>
#include <SD.h>
File root, myFile;
DHT dht(2, DHT11);
LiquidCrystal_I2C lcd(0x27, 20, 4);
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;
PID_v2 myPID(consKp, consKi, consKd, PID::Direct);
unsigned long skr = 0;
int setpoin = 335;
int t1 = 0;
int h1 = 0;
void setup() {
  Serial.begin(9600);
  dht.begin();
  analogReference(INTERNAL);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetSampleTime(10);
  myPID.Start(analogRead(A0), 0, setpoin);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("suhu oven:          ");
  lcd.setCursor(0, 1);
  lcd.print("out pwm:            ");
  lcd.setCursor(0, 2);
  lcd.print("suhu DHT11:         ");
  lcd.setCursor(0, 3);
  lcd.print("klmb. DHT11:        ");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  delay(1000);
}
void loop() {
  double sensor = analogRead(A0);
  double gap = abs(myPID.GetSetpoint() - sensor);
  if (gap < 10) {
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }
  const double output = myPID.Run(sensor);
  analogWrite(10, abs(output));
  if (output > 0) {
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
  }
  if (output < 0) {
    digitalWrite(9, LOW);
    digitalWrite(8, HIGH);
  }
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (t != t1) {
    if (t > 37) {
      digitalWrite(A1, LOW);
      digitalWrite(A2, HIGH);
      digitalWrite(A3, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      simpan(1, t, 1);
    } else if (t < 35) {
      digitalWrite(A1, HIGH);
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
      simpan(1, t, 2);
    } else {
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A3, HIGH);
      digitalWrite(5, LOW);
      simpan(1, t, 3);
    }
    t1 = t;
  }
  if (h != h1) {
    if (h > 80) {
      digitalWrite(3, HIGH);
      simpan(2, h, 1);
    } else if (h < 40) {
      digitalWrite(3, HIGH);
      simpan(2, h, 2);
    } else {
      digitalWrite(3, LOW);
      simpan(2, h, 3);
    }
  }
  h1 = h;
  if (millis() - skr > 1000) {
    lcd.setCursor(0, 0);
    lcd.print("suhu oven:          ");
    lcd.setCursor(11, 0);
    lcd.print(sensor * 0.107422);
    lcd.setCursor(0, 1);
    lcd.print("out pwm:            ");
    lcd.setCursor(9, 1);
    lcd.print(output);
    lcd.setCursor(0, 2);
    lcd.print("suhu dht11:         ");
    lcd.setCursor(12, 2);
    lcd.print(t);
    lcd.setCursor(0, 3);
    lcd.print("klmb. dht11:        ");
    lcd.setCursor(13, 3);
    lcd.print(h);
    Serial.print(setpoin);
    Serial.print(',');
    Serial.print(sensor);
    Serial.print(',');
    Serial.println(output);
    skr = millis();
  }
}
void simpan(int a, int b, int c) {
  myFile = SD.open("simpan.txt", FILE_WRITE);
  if (myFile) {
    if (a == 1) myFile.print("suhu=");
    if (a == 2) myFile.print("kelembaban=");
    myFile.print(String(b));
    myFile.print(",");
    if (c == 1) myFile.println("terlalu tinggi");
    if (c == 2) myFile.println("terlalu rendah");
    if (c == 3) myFile.println("sudah normal");
    myFile.close();
  }
}