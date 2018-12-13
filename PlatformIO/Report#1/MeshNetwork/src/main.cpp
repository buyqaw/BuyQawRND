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


#define   MESH_SSID       "BUYQAW-MESH"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       7777

// Display and Scan activities
SSD1306Wire  display(0x3c, 5, 4);
int RSSIL = 0;
BLEScan* pBLEScan = NULL;

// Prototypes
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

String signal = "";

// Schedule tasks
void scanBLE();
void sendMessage();
Task taskSendMessage( 5000, TASK_FOREVER, &sendMessage );
Task Scan_all( 5000, TASK_FOREVER, &scanBLE );


void setup() {
  Serial.begin(115200);

  BLEDevice::init("Node"); // Initialize BLE device
  // BLEDevice::setPower(ESP_PWR_LVL_P7);
  pBLEScan = BLEDevice::getScan(); //create new scan
  // pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  // Serial.println("BLE scan activated");
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  userScheduler.addTask( Scan_all );
  Scan_all.enable();
  // Serial.println("Mesh is activated");

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "BeInTech");
  display.drawString(0, 10, "Cleverest Technologies");
  display.drawString(0, 20, "Bayqaw project");
  display.drawString(0, 30, "Node ID:");
  String Level = String(mesh.getNodeId());
  display.drawStringMaxWidth(0, 40, 128, Level);
  display.display();
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}

void sendMessage() {
  String msg = "!ID: ";
  msg += mesh.getNodeId();
  msg += "- ";
  msg += signal;
  mesh.sendBroadcast(msg);
  Serial.println(msg);
  signal = "";
}


void scanBLE(){
  BLEScanResults foundDevices = pBLEScan->start(1);
  int count = foundDevices.getCount(); // Define number of found devices
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device

    if(d.haveServiceUUID()){
      for(int i = 0; i < 36; i++){
        signal += String(char(d.getServiceUUID().toString()[i]));
      }
      signal += ":";
    }
    signal += String(d.getRSSI());
    signal += ";";
  }
}

void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("%s\n", msg.c_str());
}

void newConnectionCallback(uint32_t nodeId){
  // Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  // Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
  // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  // Serial.printf("Delay to node %u is %d us\n", from, delay);
}
