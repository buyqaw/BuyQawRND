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

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
BLECharacteristic * dTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string rxValueP = "";
std::string rxValueD = "";
int scanTime = 30;

// Characteristics of WiFi
const char* ssid = "CleverestTech";
String ssidS = "CleverestTech"; // Please, dublicate it
const char* password =  "Robotics1sTheBest";

std::string SERVICE_UUID = "F0000000-0000-0000-0000-000000000000";

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
      }
    }
};

class MyCallbacksD: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValueDraw = pCharacteristic->getValue();
      if (rxValueDraw.length() > 0) {
        rxValueD = rxValueD + rxValueDraw;
      }
    }
};

// BLE server
void server(){
  std::string CHARACTERISTIC_UUID_FROM_P = SERVICE_UUID;
  CHARACTERISTIC_UUID_FROM_P[35] =  '1';
  std::string CHARACTERISTIC_UUID_FROM_D = SERVICE_UUID;
  CHARACTERISTIC_UUID_FROM_D[35] =  '2';
  // Create the BLE Device
  BLEDevice::init("Node");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_FROM_P,
											BLECharacteristic::PROPERTY_WRITE
										);
  pRxCharacteristic->setCallbacks(new MyCallbacksP());
  BLECharacteristic * dRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_FROM_D,
                      BLECharacteristic::PROPERTY_WRITE
                    );
  dRxCharacteristic->setCallbacks(new MyCallbacksD());
  // Start the service
  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Server started");
  // delay(20000);
  // pService->stop();
}

//Define priority
void define_priority(){
  Serial.println("Scanning...");
  BLEDevice::init(""); // Initialize BLE device
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount(); // Define number of found devices
  // Define local variable for UUID of parent node
  char parent[3];
  parent[2] = '\0';
  // Define local variable to priority of parent node
  int parent_max = 0;
  // For loop to define highest priority node
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    if(d.haveName()){ // If device has name
      if(d.getName() == "Node" and d.haveServiceUUID()){ // If device has our name and UUID
        std::string Parentraw = d.getServiceUUID().toString(); // Define it's UUID

        char UI[3]; UI[2] = '\0';
        // Transform std::string to char
        for(int i = 0; i<3; i++){
          UI[i] = char(Parentraw[i]);
        }
        // Define buffer to UUID transformation
        char Buffer;
        Buffer = UI[0];
        // Define value of UUID priority
        int Buf = (uint16_t)Buffer;
        // If priority is higher than local maxima than new parent is defined
        if(Buf>parent_max){
          parent_max = Buf;
          for(int m = 0; m<3; m++){
            parent[m] = UI[m];
          }
        }
      }
    }
  }
  if(parent_max>0){
    int maxi_neighbour = 0;
    if(parent[0] == 'A'){
      SERVICE_UUID[0] = '9';
    }
    else{
      SERVICE_UUID[0] = char(int(parent[0])-1);
    }
    // For loop to define same priority nodes
    for (int i = 0; i < count; i++)
    {
      BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
      // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
      if(d.haveName()){ // If device has name
        if(d.getName() == "Node" and d.haveServiceUUID()){ // If device has our name and UUID
          std::string Parentraw = d.getServiceUUID().toString(); // Define it's UUID

          char UI[3]; UI[2] = '\0';
          // Transform std::string to char
          for(int i = 0; i<3; i++){
            UI[i] = char(Parentraw[i]);
          }
          // Define buffer to UUID transformation
          char Buffer;
          Buffer = UI[0];

          if(Buffer == char(SERVICE_UUID[0])){
            if(maxi_neighbour < int(char(UI[1]))){
              maxi_neighbour = int(char(UI[1]));
            }
          }
          // If priority is higher than local maxima than new parent is defined

        }
      }
    }
    if(maxi_neighbour == 57){
      SERVICE_UUID[1] = 'A';
    }
    else{
      SERVICE_UUID[1] = char(maxi_neighbour + 1);
    }
  }
}

// Read values from other servers
void client(String parent_send, String daughter_send){
  Serial.println("Scanning...");
  BLEDevice::init(""); // Initialize BLE device
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount(); // Define number of found devices
  // Define local variable for UUID of parent node
  char parent[3];
  parent[2] = '\0';
  // Define local variable to priority of parent node
  int parent_max = 0;
  // For loop to define highest priority node
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    if(d.haveName()){ // If device has name
      if(d.getName() == "Node" and d.haveServiceUUID()){ // If device has our name and UUID
        std::string Parentraw = d.getServiceUUID().toString(); // Define it's UUID
        if(parent_send.length()>0){
          if(int(char(Parentraw[0]))>int(char(SERVICE_UUID[0]))){
            std::string adress = d.getAddress().toString();
          }
        }

      }
    }
  }
}

void setup() {
  Serial.begin(115200);

}

void loop() {
  //server();
}
