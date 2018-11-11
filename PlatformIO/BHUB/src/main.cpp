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
SSD1306Wire  display(0x3c, 5, 4);
int RSSIL = 0;
BLEScan* pBLEScan = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string rxValueP = "";
std::string rxValueD = "";
int scanTime = 1;

String macs[5] = {"0", "0", "0", "0", "0"};
String opened[5] = {"0", "0", "0", "0", "0"};

// Characteristics of WiFi
const char* ssid = "BHUB server";
const char* password =  "123123123";

// Initialize web server on 80th port
WiFiServer server(8000);


// Function to open door
void open(){
  delay(5000);
  Serial.println("Opened");
}

void voice(){
  int a = 0;
}

void add_new_user(){
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount(); // Define number of found devices
  Serial.print("Count is: ");
  Serial.println(count);
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    String mac = "24:0a:64:43:77:df";
                for (int b = 0; b < 17; b++){
                  mac[b] = d.getAddress().toString()[b];
                }
      for (int x = 0; x < 5; x++){
        if (macs[x] == "0"){
        // Serial.println(macs[x]);
      if(int(d.getRSSI()) > (-50)){ // If device has our name and UUID
        macs[x] = mac;
        voice();
        Serial.print("New user is: ");
        Serial.println(macs[x]);
      }
    break;}
    }
  }
}


// Search our devices
int scan_env(){
  voice();
  BLEScanResults foundDevices = pBLEScan->start(5);
  voice();
  int count = foundDevices.getCount(); // Define number of found devices
  Serial.print("Count is: ");
  Serial.println(count);
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    String mac = "24:0a:64:43:77:df";
                for (int b = 0; b < 17; b++){
                  mac[b] = d.getAddress().toString()[b];
                }
    if(d.haveName()){ // If device has name

      Serial.println(int(d.getRSSI()));
      if(d.getName() == "NodeL" and int(d.getRSSI()) > (-80)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
        return 1;
      }
      if(d.getName() == "Node" and int(d.getRSSI()) > (-50)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
        return 1;
      }

      }
      for (int x = 0; x < 5; x++){
        // Serial.println(macs[x]);
      if(mac == macs[x] and int(d.getRSSI()) > (-50)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
        return 1;
      }
    }
  }
  return 0;
}


void setup() {
  Serial.begin(115200);
  // Create access point
  WiFi.softAP(ssid, password);
  // Begin server
  server.begin();
  Serial.print("My IP is: ");
  Serial.println(WiFi.softAPIP());

  Serial.println("Scanning...");
  BLEDevice::init("Node"); // Initialize BLE device
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
}

void loop() {

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/0\">here</a> to add new user<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        if (currentLine.endsWith("GET /0")) {
          add_new_user();
          }
      }
    }
    // close the connection:
    client.stop();
  }else{

  int Is = scan_env();
  if(Is == 1){
    open();
  }
}
}
