#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID        "00000000-0001-0000-0000-000000000000"
#define CHARACTERISTIC_UUID "00000000-0001-0000-0000-000000000000"

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  std::string Name1 = "Mesh Node";

  BLEDevice::init(Name1);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  std::string adver = "Test 1";

  pCharacteristic->setValue(adver);
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}