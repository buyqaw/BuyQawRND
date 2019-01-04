#include <Arduino.h>            // Standard Arduino library
#include <Wire.h>               // Library to use I2C to display
#include "SSD1306Wire.h"        // Display library
#include <BLEDevice.h>          // Library to create BLE device
#include <BLEServer.h>          // Library to create BLE server
#include <BLEUtils.h>           // Library to communicate in BLE
#include <BLEScan.h>            // Library to scan BLE devices
#include <BLEAdvertisedDevice.h>// Library to advertize BLE
#include <WiFi.h>               // Library to use WiFi
#include <HTTPClient.h>         // Library to GET/POST in HTTP
#include <BLE2902.h>            // Characteristics of standard BLE device
#include <painlessMesh.h>       // Mesh network based on Wi-Fi
#include "IPAddress.h"
#include <AsyncTCP.h>


#define   MESH_SSID       "BUYQAW-MESH"
#define   MESH_PASSWORD   "beintech"
#define   MESH_PORT       7777

// Display and Scan activities
SSD1306Wire  display(0x3c, 5, 4);
int RSSIL = 0;
BLEScan* pBLEScan = NULL;
const char* ssid     = "BeInTech-Ventures";
const char* password = "BI15711588";

// Prototypes
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

String name = "";


bool calc_delay = false;
SimpleList<uint32_t> nodes;

String signal = "";

int scanTime = 1;

void send_signal(String msg){
  HTTPClient http;
  http.begin("http://10.3.178.47:7777/extra/" + msg); //Specify destination for HTTP request
  http.addHeader("Content-Type", "text/plain"); //Specify content-type header
  int httpResponseCode = http.GET(); //Send the actual POST request
  http.end(); //Free resources
}

void scanBLE(){
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  pBLEScan->setActiveScan(true);
  int count = foundDevices.getCount(); // Define number of found devices
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
      char mac[18] = "24:0a:64:43:77:df";
      for (int b = 0; b < 17; b++){
        mac[b] = d.getAddress().toString()[b];
      }
      String UUID = String(mac);
      signal += UUID;
      UUID = "";
      signal += "=";
      signal += String(d.getRSSI());
      signal += ";";
  }
  String msg = "";
  msg += name;
  msg += "!";
  msg += signal;
  send_signal(msg);
  signal = "";
}


void showit(){
  display.clear();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(25, 0, "Buyqaw project");
  display.drawString(0, 15, "Node ID:");
  String Level = String(mesh.getNodeId());
  display.drawStringMaxWidth(50, 15, 128, Level);
  display.drawString(50, 50, "Buyqaw LLP");
  display.display();
}


// Schedule tasks

Task Scan_all( 1221, TASK_FOREVER, &scanBLE );

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

  BLEDevice::init("Node"); // Initialize BLE device
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true);

  mesh.setDebugMsgTypes( ERROR );  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);

  name = String(mesh.getNodeId());
  mesh.stop();
  userScheduler.addTask( Scan_all );
  Scan_all.enable();
  // Serial.println("Mesh is activated");

  display.init();
  showit();
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}
