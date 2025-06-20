#include <PID_v2.h>
double KP = 2, KI = 5, KD = 1;
PID_v2 myPID(KP, KI, KD, PID::Direct);
unsigned long skr = 0;
int setpoin = 335;  //ADC suhu 36°C
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
  double sensor = analogRead(A0);  //nilai ADC
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