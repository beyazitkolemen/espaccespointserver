#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <WiFi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"



//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

const char* ssid = "F4BOX";
const char* password =  "f1f2f3f4";

SSD1306  display(0x3c, 4, 15);

#define DEMO_DURATION 3000
typedef void (*Demo)(void);
AsyncWebServer server(80);

int demoMode = 0;
int counter = 1;

void setup() {
  // Oled Baslatma
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  pinMode(21,OUTPUT);

 
  Serial.begin(115200);
   
 display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
WiFi.softAP(ssid, password);
 
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        digitalWrite(21, HIGH);
    request->send(SPIFFS, "/index.html", "text/html");

  });

      server.serveStatic("/jquery-3.3.1.slim.min.js", SPIFFS, "/jquery-3.3.1.slim.min.js");


      server.serveStatic("/bootstrap.min.css", SPIFFS, "/bootstrap.min.css");


    server.serveStatic("/bootstrap.min.js", SPIFFS, "/bootstrap.min.js");

  
  server.serveStatic("/1.jpg", SPIFFS, "/1.jpg");


  server.begin();

}

void drawFontFaceDemo() {

  
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, WiFi.softAPIP().toString());
    display.drawString(0, 16, "F4BOX");
    display.drawString(0, 32, "f1f2f3f4");


}



Demo demos[] = {drawFontFaceDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void loop() {
  // clear the display
  display.clear();
  // draw the current demo method
  demos[demoMode]();

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(10, 128, String(millis()));
  // write the buffer to the display
  display.display();

  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
  }
  counter++;
  delay(10);
}
