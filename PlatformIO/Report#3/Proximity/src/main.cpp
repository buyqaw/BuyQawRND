/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <WiFi.h>
#include <HTTPClient.h>


int scanTime = 1; //In seconds
const char* ssid     = "Aqbota";
const char* password = "KazPostBot";



void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
}
}


void loop() {
      BLEDevice::init("");
      BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
      pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
      BLEScanResults foundDevices = pBLEScan->start(scanTime);
      int count = foundDevices.getCount();
      String message = "";
      for (int i = 0; i < count; i++)
                {
                  BLEAdvertisedDevice d = foundDevices.getDevice(i);
                  char mac[18] = "24:0a:64:43:77:df";
                  for (int b = 0; b < 17; b++){
                    mac[b] = d.getAddress().toString()[b];
                  }
                  // client.printf("Signal from: %s, level is: ", d.getAddress().toString());
                  // Serial.printf("Signal from: %s, level is: ", d.getAddress().toString());
                  message = message + "M:" + String(mac) + "S:" + String(d.getRSSI()) + ";";
                }
      Serial.println();


HTTPClient http;
http.begin("http://192.168.8.100:7777/data/right/" + message); //Specify destination for HTTP request
http.addHeader("Content-Type", "text/plain"); //Specify content-type header
int httpResponseCode = http.GET(); //Send the actual POST request
http.end(); //Free resources
}
