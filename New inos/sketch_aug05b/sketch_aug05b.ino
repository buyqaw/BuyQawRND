#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


String connect_to_wifi(){
  // Initialize Wifi connection to the router
  char ssid[] = "Friends";     // your network SSID (name)
  char password[] = "The2ndlaw"; // your network key

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  return String(WiFi.localIP());
}

void send_tlg(String msg){
  // Initialize Telegram BOT
String BOTtoken = "699550032:AAFUE59RLRH7tKDI_uMkEfgW4ijp-hLiWwA";  // your Bot Token (Get from Botfather)
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
bot.sendMessage("274271705", msg, "");
}

void setup() {
  String IP = connect_to_wifi();
  send_tlg("Hello, I am connected to the WEB");
}

void loop() {
  delay(1000);
}

