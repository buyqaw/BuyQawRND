#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <WiFi.h>


#define SERVICE_UUID        "00000000-0001-0000-0000-000000000000"
#define CHARACTERISTIC_UUID "00000000-0001-0000-0000-000000000000"

// Function to advertise data
void server(std::string adver){
  std::string Name1 = "Mesh Node";
  BLEDevice::init(Name1);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setValue(adver);
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

// Function to connect to Wi-Fi
String connect_to_wifi(){
  // Initialize Wifi connection to the router
  char ssid[] = "CleverestTech";     // your network SSID (name)
  char password[] = "Robotics1sTheBest"; // your network key

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  return String(WiFi.localIP());
}


void setup() {
  String IP = connect_to_wifi();
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  std::string adver = "Testing the server, send some long data and LEGEN-wait4it-DARY!!!";
  server(adver);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}
