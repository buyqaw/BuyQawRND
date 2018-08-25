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

// The characteristic of services created for mesh network
static BLEUUID    charUUID_P("00000001-B5C3-F393-E0A9-E50E24DCCA9E"); // To Parent node
static BLEUUID    charUUID_D("00000002-B5C3-F393-E0A9-E50E24DCCA9E"); // To daughter node
std::string SERVICE_UUID = "F0000000-B5C3-F393-E0A9-E50E24DCCA9E"; // Service standard UUID

// Creating global variables for BLE device characteristics
static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* pRemoteCharacteristic;
BLEService *pService = NULL;
BLEServer *pServer = NULL;
BLEAdvertising* pAdvertising = NULL;
BLECharacteristic * pTxCharacteristic;
BLECharacteristic * dTxCharacteristic;

// Is devices connected or not
bool deviceConnected = false;
bool oldDeviceConnected = false;

// String for data from/to Parent and Daughters
std::string rxValueP = "";
std::string rxValueD = "";

// Variables to control mesh network
int scanTime = 10; // Scan time in seconds
int serverTime = 20000; // Server uptime in milliseconds

// Variables of mesh network
int maxi_neighbour = 0;
int isParent = 0;

// Characteristics of WiFi
const char* ssid = "CleverestTech";
String ssidS = "CleverestTech"; // Please, dublicate it
const char* password =  "Robotics1sTheBest";

// Changing device statuses if anybody connect/disconnect
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

// Callback object if parent writes anything
class MyCallbacksP: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValuePraw = pCharacteristic->getValue();
      if (rxValuePraw.length() > 0) {
        rxValueP = rxValueP + rxValuePraw;
        Serial.println("New value from parent is: ");
        for(int i = 0; i<rxValueP.length(); i++){
          Serial.print(char(rxValueP[i]));
        }
        Serial.println(";");
      }
    }
};

// Callback object if daughter writes anything
class MyCallbacksD: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValueDraw = pCharacteristic->getValue();
      if (rxValueDraw.length() > 0) {
        rxValueD = rxValueD + rxValueDraw;
        Serial.println("New value from daughter is: ");
        for(int i = 0; i<rxValueD.length(); i++){
          Serial.print(char(rxValueD[i]));
        }
        Serial.println(";");
      }
    }
};

// Function to write to parent
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

// Function to write to daughter
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

// Change data from Daughter
void DoSmtWithDataFromD(){
  rxValueD = "I am: " + SERVICE_UUID + "; Data from daughter is: " + rxValueD;
}

// Change data from parent
void DoSmtWithDataFromP(){
  rxValueP = "I am: " + SERVICE_UUID + "; Data from daughter is: " + rxValueP;
}

// BLE server (take data from daughter and parent)
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
  BLEDevice::init("Node"); // Initialize BLE device
  BLEDevice::setPower(ESP_PWR_LVL_P7); // Set power level
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime); // List of devices
  int count = foundDevices.getCount(); // Define number of found devices
  // Define local variable for UUID of parent node
  char parent[3];
  parent[2] = '\0';
  // Define local variable to priority of parent node
  int parent_max = 0;
  // For loop to define highest priority node
  if(count>0){
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device

    if(d.haveServiceUUID()){
      Serial.println("Devices are: ");
      for(int j = 0; j < 36; j++){
        Serial.print(char(d.getServiceUUID().toString()[j]));
      }
      Serial.println(";");
    }
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
      if(d.haveServiceUUID()){ // If device has our UUID
        if(char(d.getServiceUUID().toString()[9]) == char('b') and
            char(d.getServiceUUID().toString()[11]) == char('c')){
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
              int Buf = int(Buffer);
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
    if(parent[0] == 'A'){
      SERVICE_UUID[0] = '9';
    }
    else{
      SERVICE_UUID[0] = char(int(parent[0])-1);
      Serial.println(char(int(parent[0])-1));
    }
    if(char(SERVICE_UUID[0]) == 'F'){
        isParent = 1;
    }
    // For loop to define same priority nodes
    for (int i = 0; i < count; i++)
    {
      BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
      // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
        if(d.haveServiceUUID()){ // If device has our name and UUID
          if(char(d.getServiceUUID().toString()[9]) == char('b') and
              char(d.getServiceUUID().toString()[11]) == char('c')){
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
    }

    if(maxi_neighbour == 57){
      SERVICE_UUID[1] = 'A';
    }
    else{
      SERVICE_UUID[1] = char(maxi_neighbour + 1);
    }
  }
}

// Write values to other servers
void client(){
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
    if(d.haveServiceUUID()){ // If device has our UUID
      if(char(d.getServiceUUID().toString()[9]) == char('b') and
          char(d.getServiceUUID().toString()[11]) == char('c')){ // If device has our UUID
        std::string UUIDraw = d.getServiceUUID().toString(); // Define it's UUID

        // Do smt with data
        DoSmtWithDataFromD();
        DoSmtWithDataFromP();

          // If device is our parent - send data
          if(int(char(UUIDraw[0]))>int(char(SERVICE_UUID[0]))){
            std::string adress = d.getAddress().toString();
            Write2ServerP(adress, UUIDraw);
          // If device is our daughter - send data
          if(int(char(UUIDraw[0]))<int(char(SERVICE_UUID[0]))){
            std::string adress = d.getAddress().toString();
            Write2ServerD(adress, UUIDraw);
          }
        }
      }
    }
  }
}

// Standard setup function
void setup(){
  Serial.begin(115200);
  // Define priority level of our device
  define_priority();

  // Write our UUID in serial
  Serial.println("My UUID is: ");
  for(int i = 0; i<SERVICE_UUID.length(); i++){
    Serial.print(char(SERVICE_UUID[i]));
  }
  Serial.println(";");

  // Create the BLE Server to advertize our device's priority
  pServer = BLEDevice::createServer();
  Serial.println("Created BLE server device");
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.println("Callback settled");

  // Create the BLE Service for Parent node
  pService = pServer->createService(SERVICE_UUID);
  Serial.println("Created Service");
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											charUUID_P,
											BLECharacteristic::PROPERTY_WRITE
										);
  pRxCharacteristic->setCallbacks(new MyCallbacksP());
  Serial.println("Created pService for parent");

  // Create the BLE Service for daughter nodes
  BLECharacteristic * dRxCharacteristic = pService->createCharacteristic(
                      charUUID_D,
                      BLECharacteristic::PROPERTY_WRITE
                    );
  dRxCharacteristic->setCallbacks(new MyCallbacksD());

  // Start Server's service
  pService->start();

  // Add advertizing
  pAdvertising = pServer->getAdvertising();

  // Show our UUID
  pAdvertising->addServiceUUID(SERVICE_UUID);

  // Start advertizing
  pAdvertising->start();
  Serial.println("Advertize started");

  // Wait 5 minutes
  delay(300000);

  // Stop server
  pAdvertising->stop();
  Serial.println("Setting server is down");
}

// Always run loop of device
void loop(){

  // Create server to collect data
  Serial.println("I am server now for 20s, waiting for information");
  server();
  delay(1000);

  // Create client to write data
  Serial.println("I am client now, writing information to servers");
  client();
  delay(1000);

  // Zero data
  rxValueP = "";
  rxValueD = "";
}
