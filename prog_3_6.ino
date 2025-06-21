#include <SPI.h>
#include <SD.h>
String data;
bool awalan = true;
File root, myFile;
void setup() {
  Serial.begin(9600);
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
}
void loop() {
  if (Serial.available()) {
    data = Serial.readStringUntil(char(13));
    if (data.substring(0, 1) == "1") {
      myFile = SD.open(data.substring(1));
      if (myFile) {
        while (myFile.available()) {
          Serial.write(myFile.read());
        }
        myFile.close();
      }
    } else SD.remove(data.substring(1));
    awalan = true;
  } else {
    if (awalan) {
      Serial.println("Daftar file:");
      root = SD.open("/");
      while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        Serial.println(entry.name());
        entry.close();
      }
      Serial.println("Ingin baca, ketik 1 diikuti nama file");
      Serial.println("Ingin hapus, ketik 2 diikuti nama file");
      awalan = false;
    }
  }
}