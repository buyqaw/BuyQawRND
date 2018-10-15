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

// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID_P("00000001-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID    charUUID_D("00000002-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* pRemoteCharacteristic;
SSD1306Wire  display(0x3c, 5, 4);
int RSSIL = 0;
BLEScan* pBLEScan = NULL;

BLEService *pService = NULL;
BLEServer *pServer = NULL;
BLEAdvertising* pAdvertising = NULL;

BLECharacteristic * pTxCharacteristic;
BLECharacteristic * dTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string rxValueP = "";
std::string rxValueD = "";
int scanTime = 1;

// Characteristics of WiFi
const char* ssid = "CleverestTech";
String ssidS = "CleverestTech"; // Please, dublicate it
const char* password =  "Robotics1sTheBest";

std::string SERVICE_UUID = "F0000000-B5A3-F393-E0A9-E50E24DCCA9E";

// Function to open door
void open(){
  delay(5000);
}

// Search our devices
int scan_env(){
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount(); // Define number of found devices
  Serial.print("Count is: ");
  Serial.println(count);
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    if(d.haveName()){ // If device has name
      String mac = "24:0a:64:43:77:df";
                  for (int b = 0; b < 17; b++){
                    mac[b] = d.getAddress().toString()[b];
                  }
      Serial.println(int(d.getRSSI()));
      if(d.getName() == "NodeL" and int(d.getRSSI()) > (-80)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
        return 1;
      }
      if(d.getName() == "Node" and int(d.getRSSI()) > (-50)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
        return 1;
      }
      if(mac == "a4:c1:7a:57:1c:c1" and int(d.getRSSI()) > (-50)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
        return 1;
      }
      if(mac == "12:3b:6a:1b:56:77" and int(d.getRSSI()) > (-50)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
        return 1;
      }
    }
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  BLEDevice::init("Node"); // Initialize BLE device
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "BeInTech");
  display.drawString(0, 10, "Cleverest Technologies");
  display.drawString(0, 20, "BayqawSKUD project");
  display.drawString(0, 30, "Closed");
  display.display();
}

void loop() {
  int Is = scan_env();
  if(Is == 1){
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "BeInTech");
    display.drawString(0, 10, "Cleverest Technologies");
    display.drawString(0, 20, "BayqawSKUD project");
    display.drawString(0, 30, "You are welcome");
    display.display();
    open();
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "BeInTech");
    display.drawString(0, 10, "Cleverest Technologies");
    display.drawString(0, 20, "BayqawSKUD project");
    display.drawString(0, 30, "Closed");
    display.display();
  }
}
