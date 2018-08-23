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
int scanTime = 5;
int serverTime = 50000;

// Characteristics of WiFi
const char* ssid = "CleverestTech";
String ssidS = "CleverestTech"; // Please, dublicate it
const char* password =  "Robotics1sTheBest";

std::string SERVICE_UUID = "F0000000-B5A3-F393-E0A9-E50E24DCCA9E";

//Define priority
void define_priority(){
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount(); // Define number of found devices
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    if(d.haveName()){ // If device has name
      if(d.getName() == "Node"){ // If device has our name and UUID
        RSSIL = d.getRSSI();
      }
    }
    else{RSSIL = 0;}
  }

}

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  BLEDevice::init("Node"); // Initialize BLE device
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  display.init();
}

void loop() {
  define_priority();

  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "BeInTech");
  display.drawString(0, 10, "Cleverest Technologies");
  display.drawString(0, 20, "Bayqaw project");
  display.drawString(0, 30, "RSSI power:");
  String Level = String(RSSIL);
  display.drawStringMaxWidth(0, 40, 128, Level);

  display.display();
}
