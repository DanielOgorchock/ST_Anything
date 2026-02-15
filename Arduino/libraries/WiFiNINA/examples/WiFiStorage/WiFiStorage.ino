/*
  This example shows how to interact with NINA internal memory partition
  APIs are modeled on SerialFlash library (not on SD) to speed up operations and avoid buffers.
*/

#include <WiFiNINA.h>

void setup() {

  Serial.begin(115200);
  while (!Serial);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  WiFiStorageFile file = WiFiStorage.open("/fs/testfile");

  if (file) {
    file.erase();
  }

  String test = "Cantami o Diva del pelide Achille";
  file.write(test.c_str(), test.length());

  if (file) {
    file.seek(0);
    while (file.available()) {
      uint8_t buf[128];
      int ret = file.read(buf, 128);
      Serial.write(buf, ret);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
