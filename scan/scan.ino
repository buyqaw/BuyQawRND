/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 30; //In seconds

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount();

  // Making list of Mesh Nodes
  int parent = 00000000;
  int parent_id = 7777;

  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i);
    int RSSIL = d.getRSSI();
    if(d.haveName()){
      std::string Name = d.getName();
      if(Name == "Mesh Node"){
        std::string ParentRaw = d.getServiceUUID().toString();

      }
    }
    else {
      std::string Name = "No";
    }
    if (d.haveServiceUUID())
    {
      std::string UUID = d.getServiceUUID().toString();
      int UU = int(UUID.charAt(0));
      Serial.println("Scanning...");
    }
    else {
      std::string UUID = "0";
    }
    if (d.haveTXPower())
    {
      int TXP = (int)d.getTXPower();
    }
    else {
      int TXP = 0;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}