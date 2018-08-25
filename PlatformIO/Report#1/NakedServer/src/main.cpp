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
int serverTime = 50000;

// Characteristics of WiFi
const char* ssid = "CleverestTech";
String ssidS = "CleverestTech"; // Please, dublicate it
const char* password =  "Robotics1sTheBest";

std::string SERVICE_UUID = "F0000000-B0C0-F393-E0A9-E50E24DCCA9E";

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacksP: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValuePraw = pCharacteristic->getValue();
      if (rxValuePraw.length() > 0) {
        rxValueP = rxValueP + rxValuePraw;
        Serial.println("New value from parent is: ");
        for(int i = 0; i<rxValueP.length(); i++){
          Serial.print(char(rxValueP[i]));
        }
      }
    }
};

class MyCallbacksD: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValueDraw = pCharacteristic->getValue();
      if (rxValueDraw.length() > 0) {
        rxValueD = rxValueD + rxValueDraw;
        Serial.println("New value from daughter is: ");
        for(int i = 0; i<rxValueD.length(); i++){
          Serial.print(char(rxValueD[i]));
        }
      }
    }
};


void setup() {
  Serial.begin(115200);
  BLEDevice::init("Node"); // Initialize BLE device
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  std::string CHARACTERISTIC_UUID_FROM_P = SERVICE_UUID;
  CHARACTERISTIC_UUID_FROM_P[35] =  '1';
  std::string CHARACTERISTIC_UUID_FROM_D = SERVICE_UUID;
  CHARACTERISTIC_UUID_FROM_D[35] =  '2';
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  Serial.println("Created BLE server device");
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.println("Callback settled");
  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);
  Serial.println("Created Service");
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											charUUID_P,
											BLECharacteristic::PROPERTY_WRITE
										);
  pRxCharacteristic->setCallbacks(new MyCallbacksP());
  Serial.println("Created pService for parent");

  BLECharacteristic * dRxCharacteristic = pService->createCharacteristic(
                      charUUID_D,
                      BLECharacteristic::PROPERTY_WRITE
                    );
  dRxCharacteristic->setCallbacks(new MyCallbacksD());

  pService->start();
  // Start the service
  Serial.println("Server setted");
  Serial.println("Server started");
  // Start advertising
  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
  Serial.println("Advertize started");

}

void loop() {
  delay(100);
}
