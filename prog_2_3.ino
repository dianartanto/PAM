#include <PID_v2.h>
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;
PID_v2 myPID(consKp, consKi, consKd, PID::Direct);
unsigned long skr = 0;
int setpoin = 335;
void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetSampleTime(10);
  myPID.Start(analogRead(A0), 0, setpoin);
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
  if (millis() - skr > 1000) {
    Serial.print(setpoin);
    Serial.print(',');
    Serial.print(sensor);
    Serial.print(',');
    Serial.println(output);
    skr = millis();
  }
}