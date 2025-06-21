#include <WiFi.h>
#include <DHT.h>
#include <FirebaseESP32.h>
#include <HTTPClient.h>
int a = 0;
unsigned long skr = 0;
const char* ssid = "Wokwi-GUEST";
const char* password = "";
#define URL "https://rekamdata-8c407-default-rtdb.firebaseio.com" //silahkan diganti
#define SECRET "pZJJiMJuFu8sVlwD1cRm04btQV1b06fHoLla2wri"         //silahkan diganti
DHT dht(13, DHT22);
FirebaseData fbdo;
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
  dht.begin();
  firebase();
}
void loop() {
  if (millis() - skr > 2000) {
    skr = millis();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    a++;
    String apa1 = "/" + String(a) + "/s";
    String apa2 = "/" + String(a) + "/k";
    Firebase.setFloat(fbdo, apa1, t);
    Firebase.setFloat(fbdo, apa2, h);
    Serial.print(apa1);
    Serial.print(',');
    Serial.println(apa2);  
  }
}