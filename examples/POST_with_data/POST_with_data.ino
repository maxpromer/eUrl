#include <ESP8266WiFi.h>
#include <eUrl.h>

const char* ssid     = "You SSID";
const char* password = "You Pass";

eUrl eu;

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  eu.post("http://httpbin.org/post", "AA=A1&BB=B2&CC=C3", [](){
    if (eu.code()==200) {
      Serial.println("OK !");
      Serial.println("----------------------");
      Serial.println(eu.content());
      Serial.println("----------------------");
    } else if (eu.code()==0) {
      Serial.println("Error connect");
    } else {
      Serial.print("Error http code: ");
      Serial.println(eu.code());
    }
  });
}

void loop() {
  
}