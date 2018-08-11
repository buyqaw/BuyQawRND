#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID        "10000000-0001-0000-0000-000000000000"
#define CHARACTERISTIC_UUID "00000000-0001-0000-0000-000000000000"

SSD1306Wire  display(0x3c, 5, 4);

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
  delay(30000);
  pAdvertising->stop();
}

void setup() {
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "BeInTech");
  display.drawString(0, 10, "Cleverest Technologies");
  display.drawString(0, 20, "Bayqaw project");
  display.drawString(0, 30, "My UUID:");
  display.drawStringMaxWidth(0, 40, 128, "I am done");
  display.display();
  
  Serial.begin(115200);

}

void loop() {
  char msg[] = "There is going to be information about other mesh nodes from the system";
  server(msg);
}
