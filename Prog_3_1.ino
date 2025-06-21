#include <PID_v2.h>
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;
PID_v2 myPID(consKp, consKi, consKd, PID::Direct);
unsigned long skr = 0;
int setpoin1 = 496;  //121 C
int setpoin2 = 549;  //134 C
int setpoin = 0;
int s1a = 0;
void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);  // AREF = 2.5V
  pinMode(2, INPUT_PULLUP);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetSampleTime(10);
  delay(1000);
}
void loop() {
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
  if (millis() - skr > 1000) {
    Serial.print(setpoin * 0.244141);
    Serial.print(',');
    Serial.println(sensor * 0.244141);
    skr = millis();
  }
}