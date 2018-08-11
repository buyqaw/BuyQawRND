#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 30; //In seconds
int node_type = 0; // 0 - scanning, 1 - root, 2 - node

String define_UI(){
  Serial.println("Scanning...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  int count = foundDevices.getCount();
  // Making list of Mesh Nodes
  char parent[37];
  parent[36] = '\0';
  long parent_max = 0L;
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i);
    int RSSIL = d.getRSSI();
    if(d.haveName()){
      std::string Name = d.getName();
      if(Name == "Node" and d.haveServiceUUID()){
        std::string ParentRaw = d.getServiceUUID().toString();
        char UI[37]; UI[36] = '\0';
          for(int i = 0; i<=35; i++){
            UI[i] = char(ParentRaw[i]);
            }
          if (d.haveTXPower())
            {
            int TXP = (int)d.getTXPower();
            }
          char Buffer[9];
          for(int k = 0; k<8; k++){
            Buffer[k] = UI[k];
          }
          Buffer[8] = '\0';
          long Buf = atoi(Buffer);
          if(Buf>parent_max){
            parent_max = Buf;
            for(int m = 0; m<36; m++){
              parent[m] = UI[m];
            }
          }   
      }
    }
  }
  if(parent_max==0){
    String UUIDnew = "10000000-0001-0000-0000-000000000000";
  }else{
    long id = parent_max - random(1, (parent_max-(parent_max/10)));
    String id2 = String(id);
    int lenid2 = id2.length();
    for(int v = 0; v < (8 - lenid2); v++){
      id2 = "0" + id2;
    }
    char type[] = "0002"; // 0001 - for hotspot; 0002 - for nodes;
    char type2[] = "0000";
    char type3[] = "0000";
    char type4[] = "000000000000";
    String UUIDnew = String(id2) + "-" + String(type) + "-" + String(type2) + "-" + String(type3) + "-" + String(type4);
    return UUIDnew;
  }
  return "None";
}


void setup() {
  randomSeed(analogRead(0));
  Serial.begin(115200);
  String UUIDnew = define_UI();
  Serial.println("New UUID from main is:");
  Serial.println(UUIDnew);
  if(UUIDnew == "None"){
    node_type = 1;
  }
  else{
    node_type = 2;
  }
}

void loop() {
  if(node_type == 1){
        
  }
}
