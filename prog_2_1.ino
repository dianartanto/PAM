int a = 0;
void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);  //maks 1,1V
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}
void loop() {
  a++;
  if (a > 100) a = 0;
  analogWrite(10, 50);
  if (a > 50) {
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
  } else {
    digitalWrite(8, HIGH);
    digitalWrite(9, LOW);
  }
  int adc = analogRead(A0);
  float suhu = adc * 0.107422;  //dari 100*1.1/1024
  Serial.print(adc);
  Serial.print(',');
  Serial.println(suhu);
  delay(100);
}