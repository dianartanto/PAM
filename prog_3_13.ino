#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
int awal = 0;
void setup() {
  Serial.begin(9600);
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wifi not connected");
  }
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());
  config.api_key = "AIzaSyCWYRrG1msJraN1P8h7n2UONMyWi9vdvdA";
  auth.user.email = "dian.artanto@gmail.com";                               //silahkan diganti
  auth.user.password = "123456";                                            //silahkan diganti
  config.database_url = "https://simpan-62dd4-default-rtdb.firebaseio.com/";//silahkan diganti
  config.token_status_callback = tokenStatusCallback;                       //silahkan diganti
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  config.timeout.socketConnection = 10 * 1000;
  config.timeout.serverResponse = 10 * 1000;
  config.timeout.rtdbKeepAlive = 45 * 1000;
  config.timeout.rtdbStreamReconnect = 1 * 1000;
  config.timeout.rtdbStreamError = 3 * 1000;
  for (int i = 0; i < 60; i++) {
    String a = String(i) + "/a";
    String b = String(i) + "/b";
    String c = String(i) + "/c";
    String d = String(i) + "/d";
    String e = String(i) + "/e";
    String f = String(i) + "/f";
    if (Firebase.getInt(fbdo, a)) {
      Serial.print(String(i));
      Serial.print(",");
      Serial.print(fbdo.intData());
      Serial.print(",");
    }
    if (Firebase.getFloat(fbdo, b)) {
      Serial.print(fbdo.floatData());
      Serial.print(",");
    }
    if (Firebase.getFloat(fbdo, c)) {
      Serial.print(fbdo.floatData());
      Serial.print(",");
    }
    if (Firebase.getFloat(fbdo, d)) {
      Serial.print(fbdo.floatData());
      Serial.print(",");
    }
    if (Firebase.getInt(fbdo, e)) {
      Serial.print(fbdo.intData());
      Serial.print(",");
    }
    if (Firebase.getInt(fbdo, f)) {
      Serial.println(fbdo.intData());
    }
    delay(10);
  }
}
void loop() {
}