#include <Arduino.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi network name and password:
const char * networkName = "Name";
const char * networkPswd = "Password";

const char * udpBroadcastAddress = "https://remotemonitoringsystems.ca/broadcast.php";
const int udpPort = 3333;

//Are we currently connected?
boolean connected = false;

WiFiUDP udp;

void connectToWiFi(const char * ssid, const char * pwd);
void WiFiEvent(WiFiEvent_t event);

void setup() {
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println("PIR TEST");

  pinMode(36,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t pirValue = digitalRead(36);

  M5.Lcd.setCursor(60, 20, 4);
  M5.Lcd.println(pirValue);

  if(connected && pirValue == HIGH){
    udp.beginPacket(udpBroadcastAddress,udpPort);
    udp.printf("Warning!: %lu", millis()/100);
    udp.endPacket();

    delay(100);
  }   
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));
  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);  
  //Initiate connection
  WiFi.begin(ssid, pwd);
  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
      default: break;
    }
}