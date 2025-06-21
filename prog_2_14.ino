#include <WiFi.h>
#include <FirebaseESP32.h>
const char* ssid = "Wokwi-GUEST";
const char* password = "";
#define URL "https://rekamdata-8c407-default-rtdb.firebaseio.com/" //silahkan diganti
#define SECRET "pZJJiMJuFu8sVlwD1cRm04btQV1b06fHoLla2wri"          //silahkan diganti
FirebaseData fbdo;
int a = 0;
String apa1;
String apa2;
void wifiConnection() {
  WiFi.begin(ssid, password, 6);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Wifi Connected");
}
void firebase(void) {
  Firebase.begin(URL, SECRET);
  Firebase.reconnectWiFi(true);
}
void setup() {
  Serial.begin(115200);
  wifiConnection();
  firebase();
  a = 1;
  apa1 = "1/s";
  apa2 = "1/k";
}
void loop() {
  if (Firebase.getFloat(fbdo, apa1)) {
    float suhu = fbdo.floatData();
    Serial.print(suhu);
    Serial.print(',');
  }
  if (Firebase.getFloat(fbdo, apa2)) {
    float klmb = fbdo.floatData();
    Serial.println(klmb);
    a++;
    apa1 = String(a) + "/s";
    apa2 = String(a) + "/k";
  }
  delay(100);
}
