#ifndef UNIT_TEST
#include <Arduino.h>
#endif

// Arduino related
#define BAUD_RATE 115200

// Infrared sensor related
#include <IRremoteESP8266.h>
#include <IRsend.h>
#define IR_LED 4  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

// WIFI related
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#define PORT 4998

String code_on = "on";
String code_off = "off";
char sep = ':';

// WIFI authentication information
const char* ssid = "shitty2G";
const char* password = "pirate24";

WiFiServer server(4998);
WiFiClient client;

uint16_t  onData[155] = {8650,4500, 550,600, 500,600, 500,1750, 500,1750, 500,600, 550,600, 450,1800, 450,1800, 500,1750, 500,650, 450,650, 500,600, 500,650, 500,600, 500,600, 500,1800, 500,1750, 500,600, 500,600, 550,600, 500,1750, 500,600, 500,1750, 500,650, 500,600, 500,1750, 550,600, 500,600, 500,600, 500,650, 500,600, 500,650, 500,600, 500,600, 550,600, 500,600, 500,600, 550,600, 450,1800, 500,1800, 500,550, 500,650, 500,600, 500,650, 500,550, 550,600, 500,650, 500,600, 500,600, 500,650, 500,600, 500,650, 500,1750, 500,600, 500,600, 500,650, 450,650, 500,650, 500,600, 500,1750, 500,1750, 500,650, 450,650, 500,600, 500,650, 500,600, 500,600, 500,1800, 500,1750, 500,1750, 500,1750, 500,1750, 550,600, 450,1800, 500,600, 500,600, 500};
uint16_t  offData[155] = {8700,4550, 450,650, 450,650, 500,1800, 450,1750, 500,650, 450,650, 500,1800, 450,1750, 500,1800, 450,650, 450,650, 500,600, 500,650, 500,650, 450,600, 500,1800, 500,1750, 500,600, 500,650, 450,650, 500,1750, 500,600, 500,1800, 450,700, 450,600, 500,1800, 450,650, 500,600, 500,650, 450,650, 500,600, 500,650, 500,650, 450,600, 500,650, 500,650, 450,650, 450,650, 500,1800, 450,1800, 450,650, 450,650, 500,600, 500,650, 500,600, 500,650, 450,650, 500,600, 500,650, 500,600, 500,600, 500,650, 500,600, 500,650, 450,650, 500,1800, 450,650, 450,650, 500,600, 500,1800, 450,1800, 450,650, 500,600, 500,650, 500,600, 500,600, 500,650, 500,1800, 450,600, 500,1800, 450,1800, 450,650, 500,1800, 450,1800, 450,650, 450,650, 500};

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {

  // Initialize ir transmitter
  irsend.begin();

  // Initialize serial
  Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(100);

  // Initialize WIFI module
  Serial.println("");
  Serial.print("Connect to WIFI");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Success");

  // Initialize TCP Server
  server.begin();
  Serial.println("Starting TCP Server");

  // Print TCT Server Information
  IPAddress myAddress = WiFi.localIP();
  Serial.println(myAddress.toString());

}

void loop() {

  while (!client)
    client = server.available();

  while (!client.connected()) {
    delay(900);
    client = server.available();
  }
  
  while (client.available()) {
    String ir_code_str = client.readStringUntil('\r');  // Exclusive of \r
    client.readStringUntil('\n');  // Skip new line as well
    client.flush();
    
    if(ir_code_str.startsWith("code")){
      String command = getValue(ir_code_str, sep, 1);
      if(code_on == command){
        Serial.println("[On] Signal Triggered");
        irsend.sendRaw(onData, 155, 38); 
      }
      else if(code_off == command) {
        Serial.println("[Off] Signal Triggered");
        irsend.sendRaw(offData, 155, 38); 
      }
    }
  }
}
