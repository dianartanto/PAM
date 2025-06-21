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
  auth.user.email = "dian.artanto@gmail.com";                                 //silahkan diganti
  auth.user.password = "123456";                                              //silahkan diganti
  config.api_key = "AIzaSyCWYRrG1msJraN1P8h7n2UONMyWi9vdvdA";
  config.database_url = "https://simpan-62dd4-default-rtdb.firebaseio.com/";  //silahkan diganti
    config.token_status_callback = tokenStatusCallback;                       //silahkan diganti
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  config.timeout.socketConnection = 10 * 1000;
  config.timeout.serverResponse = 10 * 1000;
  config.timeout.rtdbKeepAlive = 45 * 1000;
  config.timeout.rtdbStreamReconnect = 1 * 1000;
  config.timeout.rtdbStreamError = 3 * 1000;
}
void loop() {
}
void serialEvent() {
  while (Serial.available()) {
    int data1 = Serial.parseInt();
    float data2 = Serial.parseFloat();
    float data3 = Serial.parseFloat();
    float data4 = Serial.parseFloat();
    int data5 = Serial.parseInt();
    int data6 = Serial.parseInt();
    if (Serial.read() == char(13)) {
      awal++;
      if (awal > 59) awal = 0;
      String nil1 = "/" + String(awal) + "/a";
      String nil2 = "/" + String(awal) + "/b";
      String nil3 = "/" + String(awal) + "/c";
      String nil4 = "/" + String(awal) + "/d";
      String nil5 = "/" + String(awal) + "/e";
      String nil6 = "/" + String(awal) + "/f";
      Firebase.setInt(fbdo, nil1, data1);
      Firebase.setFloat(fbdo, nil2, data2);
      Firebase.setFloat(fbdo, nil3, data3);
      Firebase.setFloat(fbdo, nil4, data4);
      Firebase.setInt(fbdo, nil5, data5);
      Firebase.setInt(fbdo, nil6, data6);
    }
  }
}