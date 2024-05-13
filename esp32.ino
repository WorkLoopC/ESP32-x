#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"
#include "sntp.h"

const char* ssid = "iPhoneHonziki";
const char* password = "esp32323232";

WebServer server(80);

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const int lcdColumns = 16;
const int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
//////////////////////////////////
void printLocalTime(){
  struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
    Serial.println("No time available (yet)");
    return;
}
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(&timeinfo, "%A,%H:%M"); 
}
void printLogo(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("TRAMECo Systems");
  }
//////////////////////////////////

void timeavailable(struct timeval *t){
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

  void handleRoot() {
server.send(200, "text/html", "<form action='/input' method='POST'><input type='text' name='message'><input type='submit' value='Submit'></form>");
}

void handleInput() {
if (server.hasArg("message")) {
  String message = server.arg("message");
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(message);
  server.send(200, "text/html", "<h1>Message received!</h1>");
  } else {
  server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  sntp_set_time_sync_notification_cb(timeavailable);
  sntp_servermode_dhcp(1); 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, handleRoot);
  server.on("/input", HTTP_POST, handleInput);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
  printLocalTime();
  delay(3000);
  printLogo();
  delay(3000);
  handleInput();
  delay(3000);
}
  
  
