#include <WiFi.h>
#include <HTTPClient.h>

int check_wifi(){
  Serial.println("scan start");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
        return 0;
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            if(WiFi.SSID(i) == "CleverestTech"){
              Serial.println("Our SSID is found");
              return 1;
            }
        }
        return 0;
    }
}

String HTTP_take(String adv){
  const char* ssid = "CleverestTech";
  const char* password =  "Robotics1sTheBest";
  char* IP = "http://192.168.1.101/";
  String response = "None";
  delay(4000);   //Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
   HTTPClient http;   
   http.begin(IP);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
   int httpResponseCode = http.POST(adv);   //Send the actual POST request
   if(httpResponseCode>0){
    response = http.getString();                       //Get the response to the request
    Serial.println(httpResponseCode);   //Print return code
   }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
   http.end();  //Free resources
 }else{
    Serial.println("Error in WiFi connection");   
 }
 WiFi.disconnect();
 return response;
}

void setup() {
  Serial.begin(115200);
  
}
void loop() {
  int check = check_wifi();
  if(check == 0){
    Serial.println("There is no our WiFi, go to sleep");
    delay(10000);
  }else{
    String adv = "POSTING from ESP32";
    String output = HTTP_take(adv);
    Serial.println(output);
  }
}
