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


// UUID of parent Node
std::string ParentRaw;

// Adress of parent Node
std::string adres = "30:AE:A4:43:03:0A";

// Initialising display of ESP32 board
SSD1306Wire  display(0x3c, 5, 4);

// Node's UUID, changes inside define_UI function
std::string SERVICE_UUID = "10000000-0001-0000-0000-000000000000";

// Node's characteristic UUID, do not change
std::string CHARACTERISTIC_UUID = "00000000-0001-0001-0000-000000000000";

// Period of scan in seconds for BLE scannning function
int scanTime = 30;

// Type of Node: 0 - scanning, 1 - root, 2 - node
int node_type = 0;

// Message send to TCP server by POST request
String msg = "First ping";

// Message from TCP server after POST request
String from_server;

// Old value of message from TCP server
String from_server_old;

// CHaracteristics of WiFi
const char* ssid = "CleverestTech";
String ssidS = "CleverestTech"; // Please, dublicate it
const char* password =  "Robotics1sTheBest";

// IP or domain of TCP server
char* IP = "http://192.168.1.101/";


// Transforming arduino String to c++ std::string type
std::string Stringtostd(String input){
   int len = input.length();
   std::string output;
    for(int v = 0; v < len; v++){
      output[v] = input[v];
    }
    return output;
}


// Transforming arduino String to char type
char* Stringtochar(String input){
   int len = input.length();
   char output[len+1];
    for(int v = 0; v < len; v++){
      output[v] = input[v];
    }
    output[len+1] = '\0';
    return output;
}


// Checking is there defined WiFi in range
int check_wifi(){
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  // If no WiFi, returns 0
  if (n == 0) {
    Serial.println("no networks found");
    return 0;
  }
  // If any WiFi, searches our
  else {
    Serial.print(n);
    Serial.println(" networks found");
    // Brootforce of WiFi list SSID
    for (int i = 0; i < n; ++i) {
      // If WiFi SSID is our, returns 1
      if(WiFi.SSID(i) == ssidS){
        Serial.println("Our SSID is found");
        return 1;
      }
    }
    return 0;
  }
}



String HTTP_take(String adv){
  String response = "None";
  delay(4000);   //Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
   HTTPClient http;
   http.begin(IP);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
   int httpResponseCode = http.POST(adv);   //Send the actual POST request
   if(httpResponseCode>0){
    response = http.getString();                       //Get the response to the request
    Serial.println(httpResponseCode);   //Print return code
   }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
   http.end();  //Free resources
 }else{
    Serial.println("Error in WiFi connection");
 }
 WiFi.disconnect();
 return response;
}


String receive_from_server(std::string adress){
// The characteristic of the remote service we are interested in.
    static BLEUUID    charUUID("00000000-0001-0001-0000-000000000000");
    String output;
    static BLEAddress *pServerAddressClient;
    static BLERemoteCharacteristic* pRemoteCharacteristicClient;

   Serial.println("Starting Arduino BLE Client application...");
   BLEDevice::init("");
   delay(1000);
   pServerAddressClient = new BLEAddress(adress);
   delay(1000);
   Serial.print("Forming a connection to ");
   Serial.println(pServerAddressClient->toString().c_str());
   BLEClient* pClient = BLEDevice::createClient();
   Serial.println("Created client");
   pClient->connect(*pServerAddressClient);
   Serial.println("Connected to server");
   // Obtain a reference to the service we are after in the remote BLE server.
   BLERemoteService* pRemoteServiceClient = pClient->getService(ParentRaw);
  Serial.println(" - Found our service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristicClient = pRemoteServiceClient->getCharacteristic(SERVICE_UUID);
  if (pRemoteCharacteristicClient == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    return "Nope";
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  std::string value = pRemoteCharacteristicClient->readValue();
  Serial.print("The characteristic value was: ");
  output = value.c_str();
  Serial.println(output);
  return output;
}



void server(char msg[]){
  std::string Name1 = "Node";
  BLEDevice::init(Name1);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );
  std::string adver = msg;
  pCharacteristic->setValue(adver);
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
  delay(10000);
  pAdvertising->stop();
}


String search_daughters(){
  Serial.println("Scanning...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount();
  // Making list of Mesh Nodes
  char parent[37];
  parent[36] = '\0';
  long parent_max = 0L;

  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i);
    int RSSIL = d.getRSSI();
    if(d.haveName()){
      std::string Name = d.getName();
      if(Name == "Node" and d.haveServiceUUID()){
        std::string Parentraw = d.getServiceUUID().toString();
        char UI[37]; UI[36] = '\0';
          for(int i = 0; i<=35; i++){
            UI[i] = char(Parentraw[i]);
            }
          char Buffer[9];
          for(int k = 0; k<8; k++){
            Buffer[k] = UI[k];
          }
          Buffer[8] = '\0';
          long Buf = atoi(Buffer);
          if(Buf>parent_max){
            parent_max = Buf;
            for(int m = 0; m<36; m++){
              parent[m] = UI[m];
            }
              adres = d.getAddress().toString();
              ParentRaw = d.getServiceUUID().toString();
          }
      }
    }
  }
  if(parent_max==0){
    String head = String(random(10000000, 99999999));
    //SERVICE_UUID = Stringtostd(head + "-0001-0000-0000-000000000000");
    return "None";

  }else{
    long id = parent_max - random(1, (parent_max-(parent_max/10)));
    String id2 = String(id);
    int lenid2 = id2.length();
    for(int v = 0; v < (8 - lenid2); v++){
      id2 = "0" + id2;
    }
    char type[] = "0002"; // 0001 - for hotspot; 0002 - for nodes;
    char type2[] = "0000";
    char type3[] = "0000";
    char type4[] = "000000000000";
    String UUIDnew = String(id2) + "-" + String(type) + "-" + String(type2) + "-" + String(type3) + "-" + String(type4);
    SERVICE_UUID = Stringtostd(UUIDnew);
    return UUIDnew;
  }
  return "None";
}


String define_UI(){
  Serial.println("Scanning...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount();
  // Making list of Mesh Nodes
  char parent[37];
  parent[36] = '\0';
  long parent_max = 0L;
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i);
    int RSSIL = d.getRSSI();
    if(d.haveName()){
      std::string Name = d.getName();
      if(Name == "Node" and d.haveServiceUUID()){
        std::string Parentraw = d.getServiceUUID().toString();
        char UI[37]; UI[36] = '\0';
          for(int i = 0; i<=35; i++){
            UI[i] = char(Parentraw[i]);
            }
          char Buffer[9];
          for(int k = 0; k<8; k++){
            Buffer[k] = UI[k];
          }
          Buffer[8] = '\0';
          long Buf = atoi(Buffer);
          if(Buf>parent_max){
            parent_max = Buf;
            for(int m = 0; m<36; m++){
              parent[m] = UI[m];
            }
              adres = d.getAddress().toString();
              ParentRaw = d.getServiceUUID().toString();
          }
      }
    }
  }
  if(parent_max==0){
    String head = String(random(10000000, 99999999));
    SERVICE_UUID = Stringtostd(head + "-0001-0000-0000-000000000000");

  }else{
    long id = parent_max - random(1, (parent_max-(parent_max/10)));
    String id2 = String(id);
    int lenid2 = id2.length();
    for(int v = 0; v < (8 - lenid2); v++){
      id2 = "0" + id2;
    }
    char type[] = "0002"; // 0001 - for hotspot; 0002 - for nodes;
    char type2[] = "0000";
    char type3[] = "0000";
    char type4[] = "000000000000";
    String UUIDnew = String(id2) + "-" + String(type) + "-" + String(type2) + "-" + String(type3) + "-" + String(type4);
    SERVICE_UUID = Stringtostd(UUIDnew);
    return UUIDnew;
  }
  return "None";
}


void setup() {
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "BeInTech");
  display.drawString(0, 10, "Cleverest Technologies");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Searching for wifi");
  display.display();
  randomSeed(analogRead(0));
  Serial.begin(115200);
  int check = check_wifi();
  if(check == 1){
    Serial.println("We are parent from know");
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "BeInTech");
    display.drawString(0, 10, "Cleverest Technologies");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 20, "We found wifi");
    display.display();
    node_type = 1;
  }else{
    String UUIDnew = define_UI();
    Serial.println("New UUID from main is:");
    Serial.println(UUIDnew);

    if(UUIDnew == "None"){
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "BeInTech");
      display.drawString(0, 10, "Cleverest Technologies");
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 20, "There is no parent or WiFi");
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 40, "I go to reboot");
      display.display();
      delay(10000);
      ESP.restart();
      }
    else{
      node_type = 2;
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "BeInTech");
      display.drawString(0, 10, "Cleverest Technologies");
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 20, "There is parent");
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 40, "I will try to connect");
      display.display();
    }
  }
  delay(10000);
}


void loop() {
  if(node_type == 1){
    // Data from server to advertize
        from_server = HTTP_take(msg);
        if(from_server == from_server_old){
          from_server = "None";
        }else{
          from_server_old = from_server;
        }
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, "BeInTech");
        display.drawString(0, 10, "Cleverest Technologies");
        display.drawString(0, 20, "I am parent node");
        display.drawString(0, 30, "My UUID:");
        char maUID[37];
        for(int ka = 0; ka < 37; ka++){
          maUID[ka] = SERVICE_UUID[ka];
        }
        display.drawStringMaxWidth(0, 40, 128, maUID);
        display.display();
        server(Stringtochar(from_server));

  }
  else{
    String data1 = receive_from_server(adres);
    Serial.println(data1);
  }
}
