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
int scanTime = 10;
int serverTime = 20000;

// Characteristics of WiFi
const char* ssid = "CleverestTech";
String ssidS = "CleverestTech"; // Please, dublicate it
const char* password =  "Robotics1sTheBest";

std::string SERVICE_UUID = "F0000000-B5A3-F393-E0A9-E50E24DCCA9E";

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

void Write2ServerP(std::string adress, std::string serviceUUID){
  pServerAddress = new BLEAddress(adress);
  delay(1000);
  Serial.print("Forming a connection to ");
  Serial.println(pServerAddress->toString().c_str());
  BLEClient* pClient = BLEDevice::createClient();
  Serial.println("Created client");
  pClient->connect(*pServerAddress);
  Serial.println("Connected to server");
  // Obtain a reference to the service we are after in the remote BLE server.
 BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
 if (pRemoteService == nullptr) {
   Serial.print("Failed to find our service UUID");
 }
 Serial.println(" - Found our service");

 // Obtain a reference to the characteristic in the service of the remote BLE server.
 pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID_P);
 if (pRemoteCharacteristic == nullptr) {
   Serial.print("Failed to find our characteristic UUID");
 }
 Serial.println(" - Found our characteristic");

 pRemoteCharacteristic->writeValue(rxValueD.c_str(), rxValueD.length());
 Serial.println("I wrote to parent this:");
 for(int i = 0; i<rxValueD.length(); i++){
   Serial.print(char(rxValueD[i]));
 }
 Serial.println(";");
 //pClient->clearServices();
}

void Write2ServerD(std::string adress, std::string serviceUUID){
  pServerAddress = new BLEAddress(adress);
  delay(1000);
  Serial.print("Forming a connection to ");
  Serial.println(pServerAddress->toString().c_str());
  BLEClient* pClient = BLEDevice::createClient();
  Serial.println("Created client");
  pClient->connect(*pServerAddress);
  Serial.println("Connected to server");
  // Obtain a reference to the service we are after in the remote BLE server.
 BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
 if (pRemoteService == nullptr) {
   Serial.print("Failed to find our service UUID");
 }
 Serial.println(" - Found our service");

 // Obtain a reference to the characteristic in the service of the remote BLE server.
 pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID_D);
 if (pRemoteCharacteristic == nullptr) {
   Serial.print("Failed to find our characteristic UUID");
 }
 Serial.println(" - Found our characteristic");

 pRemoteCharacteristic->writeValue(rxValueP.c_str(), rxValueP.length());
 Serial.println("I wrote to daughter this:");
 for(int i = 0; i<rxValueP.length(); i++){
   Serial.print(char(rxValueP[i]));
 }
 Serial.println(";");
}

void DoSmtWithDataFromD(){
  rxValueD = "I am: " + SERVICE_UUID + "; Data from daughter is: " + rxValueD;
}

void DoSmtWithDataFromP(){
  rxValueP = "I am: " + SERVICE_UUID + "; Data from daughter is: " + rxValueP;
}

// BLE server
void server(){

  Serial.println("Server started");
  // Start advertising
  pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Advertize started");
  delay(serverTime);
  pAdvertising->stop();
}

//Define priority
void define_priority(){
  Serial.println("Scanning...");
  BLEDevice::init("Node"); // Initialize BLE device
  BLEDevice::setPower(ESP_PWR_LVL_P7);
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
        Serial.println("Here is our device");
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
        Serial.print("Buf is: ");
        Serial.println(Buf);
        // If priority is higher than local maxima than new parent is defined
        if(Buf>parent_max){
          parent_max = Buf;
          Serial.print("Parentmax is: ");
          Serial.println(parent_max);
          for(int m = 0; m<3; m++){
            parent[m] = UI[m];
          }
        }
      }
    }
  }
  if(parent_max>0){
    Serial.print("Parent UUID is: ");
    int maxi_neighbour = 0;
    if(parent[0] == 'A'){
      SERVICE_UUID[0] = '9';
    }
    else{
      SERVICE_UUID[0] = char(int(parent[0])-1);
      Serial.println(char(int(parent[0])-1));
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
void client(){
  Serial.println("Scanning...");

  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount(); // Define number of found devices
  Serial.println("Number of found devices is:");
  Serial.println(count);
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
        std::string UUIDraw = d.getServiceUUID().toString(); // Define it's UUID

        DoSmtWithDataFromD();
        DoSmtWithDataFromP();

          if(int(char(UUIDraw[0]))>int(char(SERVICE_UUID[0]))){
            std::string adress = d.getAddress().toString();
            Write2ServerP(adress, UUIDraw);

          if(int(char(UUIDraw[0]))<int(char(SERVICE_UUID[0]))){
            std::string adress = d.getAddress().toString();
            Write2ServerD(adress, UUIDraw);
          }

        }

      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  define_priority();
  Serial.println("My UUID is: ");
  for(int i = 0; i<SERVICE_UUID.length(); i++){
    Serial.print(char(SERVICE_UUID[i]));
  }
  Serial.println(";");

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
  pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Advertize started");
  delay(50000);
  pAdvertising->stop();
  // Start the service
  Serial.println("Server setted");

}

void loop() {
  Serial.println("I am server now for 20s, waiting for information");
  server();
  rxValueP = "";
  rxValueD = "";
  delay(1000);
  Serial.println("I am client now, writing information to servers");
  client();
  delay(1000);
}
