#include <PID_v2.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
DHT dht(2, DHT11);
LiquidCrystal_I2C lcd(0x27, 20, 4);
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;
PID_v2 myPID(consKp, consKi, consKd, PID::Direct);
unsigned long skr = 0;
int setpoin = 335;
void setup() {
  Serial.begin(9600);
  dht.begin();
  analogReference(INTERNAL);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetSampleTime(10);
  myPID.Start(analogRead(A0), 0, setpoin);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("suhu oven: ");
  lcd.setCursor(0, 1);
  lcd.print("out pwm: ");
  lcd.setCursor(0, 2);
  lcd.print("suhu DHT11: ");
  lcd.setCursor(0, 3);
  lcd.print("klmb. DHT11: ");
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
  if (millis() - skr > 1000) {
    lcd.setCursor(0, 0);
    lcd.print("suhu oven: ");
    lcd.setCursor(11, 0);
    lcd.print(sensor * 0.107422);
    lcd.setCursor(0, 1);
    lcd.print("out pwm: ");
    lcd.setCursor(9, 1);
    lcd.print(output);
    lcd.setCursor(0, 2);
    lcd.print("suhu dht11: ");
    lcd.setCursor(12, 2);
    lcd.print(t);
    lcd.setCursor(0, 3);
    lcd.print("klmb. dht11: ");
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