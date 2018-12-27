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

int MIN = -50;
int MAX = -90;

bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string rxValueP = "";
std::string rxValueD = "";
int scanTime = 1;

String macs[5] = {"0", "0", "0", "0", "0"};
String opened[5] = {"0", "0", "0", "0", "0"};

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

void add_new_user(){
  Serial.println("Adding new users");
  digitalWrite (red, HIGH);
  digitalWrite (blue, LOW);
  BLEScanResults foundDevices = pBLEScan->start(5);
  int count = foundDevices.getCount(); // Define number of found devices
  Serial.print("Device`s number is: ");
  Serial.println(count);
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i); // Define found device
    // int RSSIL = d.getRSSI(); // Get it's signal level [no need now, but for future]
    String mac = "24:0a:64:43:77:df";
                for (int b = 0; b < 17; b++){
                  mac[b] = d.getAddress().toString()[b];
                }
      Serial.print("Device`s mac address: ");
      Serial.println(mac);
      Serial.print("Device`s RSSI is: ");
      Serial.println(d.getRSSI());
      for (int x = 0; x < 5; x++){
        if (macs[x] == "0"){
          Serial.print("There is place for mac addr: ");
          Serial.print("Index is - ");
          Serial.print(x);
          Serial.print(", value is - ");
          Serial.println(macs[x]);
        // Serial.println(macs[x]);
      if(int(d.getRSSI()) > (MIN)){ // If device has our name and UUID
        Serial.print("New user`s mac adrr: ");
        macs[x] = mac;
        Serial.println(mac);
        voice();
        Serial.print("New user is: ");
        Serial.println(macs[x]);
      }
    digitalWrite (blue, HIGH);
    digitalWrite (red, LOW);
    break;}
    }
  }
  Serial.println("Finished adding new users");
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
    String mac = "24:0a:64:43:77:df";
                for (int b = 0; b < 17; b++){
                  mac[b] = d.getAddress().toString()[b];
                }
    if(d.haveName()){ // If device has name

      Serial.println(int(d.getRSSI()));
      if(d.getName() == "NodeL" and int(d.getRSSI()) > (MAX)){ // If device has our name and UUID
        return 1;
      }
      if(int(d.getRSSI()) > (MIN)){ // If device has our name and UUID
        return 1;
      }

      }
      for (int x = 0; x < 5; x++){
        // Serial.println(macs[x]);
      if(mac == macs[x] and int(d.getRSSI()) > (MIN)){ // If device has our name and UUID
        Serial.println(int(d.getRSSI()));
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
  Serial.println("Scanning...");
  BLEDevice::init("Node"); // Initialize BLE device
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  add_new_user();
}

void loop() {
  int Is = scan_env();
  if(Is == 1){
    open();
}
}
