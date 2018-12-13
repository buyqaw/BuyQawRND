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
#include <ESPAsyncWebServer.h>


// Settings

String worker = "0000fef5-0000-1000-8000-00805f9b34fb";
String mac = "24:0a:64:43:77:df";
int MIN = -70;
int MAX = -90;
int INROOM = -80;

#define   MESH_SSID       "BUYQAW-MESH"
#define   MESH_PASSWORD   "beintech"
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
AsyncWebServer server(80);

IPAddress myAPIP(0,0,0,0);

bool calc_delay = false;
SimpleList<uint32_t> nodes;
std::vector<int> datas;

String signal = "";
String data = "Hello World";


// Schedule tasks
void scanBLE();
void sendMessage();
void Skud();

Task taskSendMessage( 5000, TASK_FOREVER, &sendMessage );
Task Scan_all( 5000, TASK_FOREVER, &scanBLE );
Task SKUD( 500, TASK_FOREVER, &Skud );



int scanTime = 1;


const int speak = 16;
const int door = 17;
const int red = 27;
const int green = 14;
const int blue = 12;

// Function to open door
void open(){
  digitalWrite (door, LOW);
  digitalWrite (speak, HIGH);
  digitalWrite (blue, LOW);
  digitalWrite (green, HIGH);
  Serial.println("Opened");
  delay(5000);
  digitalWrite (blue, HIGH);
  digitalWrite (green, LOW);
  digitalWrite (door, HIGH);
  digitalWrite (speak, LOW);
}

void voice(){
  digitalWrite (speak, HIGH);
  delay(1000);
  digitalWrite (speak, LOW);

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

          for (int b = 0; b < 17; b++){
                  mac[b] = d.getAddress().toString()[b];
                }
    if(d.haveName()){ // If device has name

      Serial.println(int(d.getRSSI()));
      if(d.getName() == "NodeL" and int(d.getRSSI()) > (MAX)){ // If device has our name and UUID
        return 1;
      }
      if(d.getName() == "Node" and int(d.getRSSI()) > (MIN)){ // If device has our name and UUID
        return 1;
      }

      }
  }
  return 0;
}


void setup() {
  pinMode (red, OUTPUT);
  pinMode (blue, OUTPUT);
  pinMode (green, OUTPUT);
  pinMode (speak, OUTPUT);
  pinMode (door, OUTPUT);
  digitalWrite (door, HIGH);
  digitalWrite (blue, HIGH);

  Serial.begin(115200);
  datas.push_back(-200);

  BLEDevice::init("Node"); // Initialize BLE device
  pBLEScan = BLEDevice::getScan(); //create new scan
  mesh.setDebugMsgTypes(ERROR);  // set before init() so that you can see startup messages

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

  myAPIP = IPAddress(mesh.getAPIP());

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Bayqaw project");
  display.drawString(0, 10, "Node ID:");
  String Level = String(mesh.getNodeId());
  display.drawStringMaxWidth(0, 20, 128, Level);
  String ip = myAPIP.toString();
  display.drawString(0, 30, "Server IP:");
  display.drawStringMaxWidth(0, 40, 128, ip);
  display.display();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    int len = datas.size();
    int k = 0;
    int max = -200;
    int indexofmax = 0;
    while(k<len){
      if(datas[k]>max){
        max = datas[k];
        indexofmax = k;
      }
    }
    if(max>INROOM){
      data = "Worker is in the room number: <b>";
      std::list<uint32_t> modes = mesh.getNodeList();
      int sizeofnodes = mesh.getNodeList().size();
      int i = 0;
      std::vector<uint32_t> nodesinV(modes.begin(), modes.end());
      data += String(nodesinV[indexofmax]);
      data += "</b>";
    }
    else{
      data = "Worker`s spot is unknown";
    }
    request->send(200, "text/html", data);
  });
  server.begin();
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}

void sendMessage() {
  String msg = "";
  msg += mesh.getNodeId();
  msg += ":";
  msg += signal;
  mesh.sendBroadcast(msg);
  data = msg;
  signal = "";
}


void scanBLE(){
  BLEScanResults foundDevices = pBLEScan->start(1);
  int count = foundDevices.getCount(); // Define number of found devices
  bool was = 0;
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device

    if(d.haveServiceUUID()){
      String UUID = "";
      for(int i = 0; i < 36; i++){
        UUID += String(char(d.getServiceUUID().toString()[i]));
      }

      if(UUID == worker){
        datas[0] = int(d.getRSSI());
        was = 1;
      }

      signal += UUID;
      UUID = "";
      signal += ":";
      signal += String(d.getRSSI());
      signal += ";";
    }
  }

  if(was == 0){
    datas[0] = -200;
  }
}

void receivedCallback(uint32_t from, String & msg) {
  int k = msg.indexOf(':');
  String msgN = msg.substring(k);
  uint32_t id = uint32_t(msg.substring(0, k).toFloat());
  std::list<uint32_t> modes = mesh.getNodeList();
  int sizeofnodes = mesh.getNodeList().size();
  int i = 0;
  std::vector<uint32_t> nodesinV(modes.begin(), modes.end());
  while(i<sizeofnodes){
    uint32_t baba = uint32_t(nodesinV[i]);
    if(baba == id){
      while(1){
        int m = msgN.indexOf(':');
        String msgW = msgN.substring(0,m);
        msgN = msgN.substring(m);
        int a = msgN.indexOf(';');
        int RSSI = msgN.substring(0,a).toInt();
        if(msgW == worker){
          datas[i+1] = RSSI;
          break;
        }
        else{
          datas[i+1] = -200;
        }
      }
      break;
    }
    i += 1;
  }
}

void newConnectionCallback(uint32_t nodeId){
  datas.push_back(-200);
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

void Skud() {
  int Is = scan_env();
  if(Is == 1){
    open();
  }
}
