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
std::string adres;

// Initialising display of ESP32 board
SSD1306Wire  display(0x3c, 5, 4);

// Node's UUID, changes inside define_UI function
std::string SERVICE_UUID;

// Period of scan in seconds for BLE scannning function
int scanTime = 30;

// Type of Node: 0 - scanning, 1 - root, 2 - node
int node_type = 0;

// Message send to TCP server by POST request
String msg = "First ping";

// Message from TCP server after POST request
String from_server;

// Characteristics of WiFi
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

// Function to connect to server, and send post and get data = return String
String HTTP_take(String adv){
  // Initialize String with some value to allocate some memory
  String response = "None";
  delay(4000); // Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password); // Begin WiFi to connect to internet
  while (WiFi.status() != WL_CONNECTED) { // Connecting to WiFi
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // WiFi is connected
  Serial.println("Connected to the WiFi network");

  if(WiFi.status()== WL_CONNECTED){   // Check WiFi connection status
    HTTPClient http;
    http.begin(IP);  // Specify destination for HTTP request
    http.addHeader("Content-Type", "text/plain");             // Specify content-type header
    int httpResponseCode = http.POST(adv);   // Send the actual POST request
    if(httpResponseCode>0){
      response = http.getString(); //Get the response to the request
      Serial.println(httpResponseCode); // Print return code
    }
    else{
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end(); // Free memory resources
   }
   else{
     Serial.println("Error in WiFi connection");
   }
   WiFi.disconnect(); // Disconnect from WiFi
   return response; // Return value
}

// Function to connect to server and receive data
String receive_from_server(std::string adress, std::string UUID_given){
    String output; // Initialize ouput as string
    static BLEAddress *pServerAddressClient; // Initialize adress variable
    static BLERemoteCharacteristic* pRemoteCharacteristicClient; // Initialize charasteristic (UUID) variable
    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init(""); // Initialize BLE device
    delay(1000); // Give time to POSIX to Initialize
    pServerAddressClient = new BLEAddress(adress); // Give adress to BLE adress variable
    delay(1000);
    Serial.print("Forming a connection to ");
    Serial.println(pServerAddressClient->toString().c_str());
    BLEClient* pClient = BLEDevice::createClient(); // Creating BLE client
    Serial.println("Created client");
    pClient->connect(*pServerAddressClient); // Connecting to BLE server
    Serial.println("Connected to server");
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteServiceClient = pClient->getService(UUID_given);
    Serial.println(" - Found our service");
    // Obtain a reference to the characteristic in the service of the remote BLE server
    pRemoteCharacteristicClient = pRemoteServiceClient->getCharacteristic(UUID_given);
    if (pRemoteCharacteristicClient == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      return "Nope";
    }
    Serial.println(" - Found our characteristic");
    // Read the value of the characteristic
    std::string value = pRemoteCharacteristicClient->readValue();
    Serial.print("The characteristic value was: ");
    output = value.c_str();
    Serial.println(output);
    return output;
}

// Create and transmit msg as BLE server
void server(char msg[]){
    // The name of BLE server
    std::string Name1 = "Node";
    BLEDevice::init(Name1); // Creating BLE device
    Serial.println("Our name given ");
    BLEServer *pServer = BLEDevice::createServer(); // Creating BLE server
    Serial.println("Server defined");

    char UI[37]; UI[36] = '\0';
      for(int i = 0; i<36; i++){
        UI[i] = char(SERVICE_UUID[i]);
        }
    Serial.println(UI);

    // TODO: THIS LINE is bugged, find problem
    BLEService *pService = pServer->createService(SERVICE_UUID); // Using SERVICE_UUID as UUID of server

    Serial.println("UUID is given");
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                          SERVICE_UUID, // UUID of server, changes in function define_UI
                                          BLECharacteristic::PROPERTY_READ // Give only read characteristic
                                        );

    std::string adver = msg; // From char to std::string value
    pCharacteristic->setValue(adver); // Define advertize message for Daughter

    pService->start(); // Start service
    // Define characteristics of server
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start(); // Start advertize
    delay(20000); // Advertize for 20 seconds
    pAdvertising->stop(); // Stop it
}

// Function to search daughters (Nodes with lower priority), return their messages
String from_daughters(){
  Serial.println("Scanning...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount();

  // Making list of Mesh Nodes
  char daughter[37];
  daughter[36] = '\0';

  // Define string to output
  String output;

  // Define our priority
  char UI[37]; UI[36] = '\0';
    for(int i = 0; i<=35; i++){
      UI[i] = char(SERVICE_UUID[i]);
      }
    char Buffer[9];
    for(int k = 0; k<8; k++){
      Buffer[k] = UI[k];
    }
  Buffer[8] = '\0';
  long our_priority = atoi(Buffer);

  // Check if node is our daughter
  // If it is than store message from it
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i);
    if(d.haveName()){
      std::string Name = d.getName();
      if(Name == "Node" and d.haveServiceUUID()){
        std::string Draw = d.getServiceUUID().toString();
        char UI[37]; UI[36] = '\0';
          for(int i = 0; i<=35; i++){
            UI[i] = char(Draw[i]);
            }
          char Buffer[9];
          for(int k = 0; k<8; k++){
            Buffer[k] = UI[k];
          }
          Buffer[8] = '\0';
          long Buf = atoi(Buffer);
          if(Buf<our_priority){
            std::string Dadres = d.getAddress().toString();
            std::string DUUID = d.getServiceUUID().toString();
            output = output + receive_from_server(Dadres, DUUID);
          }
      }
    }
  }
  // Return messages values
  return output;
}

// Function to search parent node, if no = parent node
String define_UI(){
  Serial.println("Scanning...");
  BLEDevice::init(""); // Initialize BLE device
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount(); // Define number of found devices
  // Making list of Mesh Nodes
  // Define local variable for UUID of parent node
  char parent[37];
  parent[36] = '\0';

  // Define local variable to priority of parent node
  long parent_max = 0L;

  // For loop to define highest priority node
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    if(d.haveName()){ // If device has name

      std::string Name = d.getName();

      if(Name == "Node" and d.haveServiceUUID()){ // If device has our name and UUID

        std::string Parentraw = d.getServiceUUID().toString(); // Define it's UUID

        char UI[37]; UI[36] = '\0';
        // Transform std::string to char
        for(int i = 0; i<=35; i++){
          UI[i] = char(Parentraw[i]);
        }
        // Define buffer to UUID transformation
        char Buffer[9];
        for(int k = 0; k<8; k++){
          Buffer[k] = UI[k];
        }
        Buffer[8] = '\0';
        // Define value of UUID priority
        long Buf = atoi(Buffer);
        // If priority is higher than local maxima than new parent is defined
        if(Buf>parent_max){
          parent_max = Buf;
          for(int m = 0; m<36; m++){
            parent[m] = UI[m];
          }
            // Set new adres to variable adres
            adres = d.getAddress().toString();
            // Set new UUID to variable ParentRaw
            ParentRaw = d.getServiceUUID().toString();
        }
      }
    }
  }
  // If there is no parent than be parent
  if(parent_max==0){
    return "None";
  }
  else{
    // Create new random lower priority
    long id = parent_max - random(1, (parent_max-(parent_max/10)));
    // Transform it to String
    String id2 = String(id);
    // Add leading zeros is needed
    int lenid2 = id2.length();
    for(int v = 0; v < (8 - lenid2); v++){
      id2 = "0" + id2;
    }
    // Create other values of UUID
    char type[] = "0002"; // 0001 - for hotspot; 0002 - for nodes;
    char type2[] = "0000";
    char type3[] = "0000";
    char type4[] = "000000000000";
    // Define new UUID
    String UUIDnew = String(id2) + "-" + String(type) + "-" + String(type2) + "-" + String(type3) + "-" + String(type4);
    SERVICE_UUID = Stringtostd(UUIDnew);
    return UUIDnew;
  }
  // I do not know what it is, it is for magic
  return "None";
}

// Standard setup function
void setup() {
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "BeInTech");
  display.drawString(0, 10 /* This value is hight of the text location */, "Cleverest Technologies");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Searching for wifi");
  display.display();

  // Random seed for random function from analogue read from empty pin
  randomSeed(analogRead(0));

  // Serial begin at rate 115200
  Serial.begin(115200);

  // Check is there WiFi for our node
  int check = check_wifi();

  // If there WiFi = be parent node
  if(check == 1){
    node_type = 1; // Define node type
    String head = String(random(10000000, 99999999));
    head = head + "-0001-0000-0000-000000000000";
    SERVICE_UUID = Stringtostd(head);
  }
  // If there is no WiFi try to find parent node and be daughter
  else{
    // Searching for parent node
    String UUIDnew = define_UI();
    Serial.println("New UUID from main is:");
    Serial.println(UUIDnew);

    // If there is no parent node and WiFi - reboot after 100 sec
    if(UUIDnew == "None"){
      delay(100000);
      ESP.restart();
    }
    // If there is parent try to connect
    else{
      node_type = 2; // Define node type as daughter
    }
  }
  delay(5000);
  Serial.println("Setup is done");


}

// Loop of node
void loop() {
  Serial.println("Loop is started");
  // If node is parent than:
  // 1. Read value from HTTP server
  // 2. Transmit it to daughters
  // 3. Read their feedback and store it in one string
  // 4. Send it to server as POST request
  // 5. Repeat
  // 6. ???
  // 7. Profit!!!
  if(node_type == 1){
    // Data from HTTP server to advertize
    from_server = HTTP_take(msg);

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "BeInTech");
    display.drawString(0, 10, "Cleverest Technologies");
    display.drawString(0, 20, "I am parent node");
    display.display();

    // BLE server which sends value from HTTP server
    server(Stringtochar(from_server));

    // Search for daughters and receive their values
    msg = from_daughters();
    // Than it goes to begining of loop with new msg value
  }
  // If node is daughter than do:
  // 1. Receive data from parent node and daughter nodes
  // 2. Send data to daughter nodes and parent node
  else{
    // Display some info
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "BeInTech");
    display.drawString(0, 10, "Cleverest Technologies");
    display.drawString(0, 20, "I am daughter node");
    display.drawString(0, 30, "My UUID:");
    char maUID[37];
    for(int ka = 0; ka < 37; ka++){
      maUID[ka] = SERVICE_UUID[ka];
    }
    display.drawStringMaxWidth(0, 40, 128, maUID);
    display.display();

    // Search for parent node and receive data from it
    String data1 = receive_from_server(adres, ParentRaw);
    Serial.println("Data from BLE server is: ");
    Serial.println(data1);
    // Create some message
    String msg1;
    for(int ka = 0; ka < 37; ka++){
      msg1[ka] = SERVICE_UUID[ka];
    }
    msg1 = msg1 + " - I am okay, how about you?;";
    // Search for daughters and receive their values
    msg1 = msg1 + from_daughters();
    Serial.println("Data to BLE server is: ");
    Serial.println(msg1);
    // Create BLE server with 2 services, one for daughters and one for parent
    server(Stringtochar(msg1));
  }
}
