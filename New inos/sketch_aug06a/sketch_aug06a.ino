#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

void setup() {
  // put your setup code here, to run once:
  std::string Baby = "01234567";
  char baby[8];
  for(int i = 0; i<=7; i++){
  baby[i] = char(Baby[i]);
  }
  Serial.begin(115200);
  Serial.println(baby);
}

void loop() {
  // put your main code here, to run repeatedly:

}
