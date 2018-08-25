#include "BLEDevice.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("10000000-0001-0000-0000-000000000000");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("00000000-0001-0000-0000-000000000000");
String output;
static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* pRemoteCharacteristic;


String receive_from_server(char adres[]){
  
   Serial.println("Starting Arduino BLE Client application...");
   BLEDevice::init("");
   delay(1000);
   std::string adress = adres;
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
    Serial.print("Failed to find our service UUID: ");
    return "Nope";
    Serial.println(serviceUUID.toString().c_str());
  }
  Serial.println(" - Found our service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    return "Nope";
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  std::string value = pRemoteCharacteristic->readValue();
  Serial.print("The characteristic value was: ");
  output = value.c_str();
  Serial.println(output);
  return output;
}


void setup() {
   Serial.begin(115200);
   char adres[] = "30:AE:A4:43:03:0A";
   String data1 = receive_from_server(adres);
   Serial.println("Data is: ");
   Serial.println(data1);
}
void loop(){
delay(1000);
}

