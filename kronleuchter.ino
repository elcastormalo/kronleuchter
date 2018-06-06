#define SSID_ME "fnordeing ang"
#define PW_ME "R112Zr11ch3353burger"
#define HOST_ME "Kronleuchter"

// Analog in fuer Lautstaerke A0 auf NodeMCU
#define PIN_ANALOG_IN A0
// Leds Pin D4 auf NodeMCU
#define DATA_PIN_STRIP    2  // Fucking PIN 4 in der FastLed Library
#define LED_TYPE    WS2811
#define COLOR_ORDER NEO_RGBW
#define MAX_LEDS    80
#define ROWS        8
#define ZACKEN      6
#define BRIGHTNESS          50   // initial brightness
#define FRAMES_PER_SECOND  60

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>          // Board Manager:http://arduino.esp8266.com/stable/package_esp8266com_index.json
#include <DNSServer.h>            // Local DNS Server used for redirecting all requests to the configuration portal. Kommt mit ESP8266Wifi Board
#include <WiFiManager.h>          // Im Librarymanager installierbar https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266HTTPUpdateServer.h> // Kommt mit ESP8266Wifi Board
#include <ESP8266mDNS.h>


ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
const char* host = HOST_ME;
const char* ssid     = SSID_ME;
const char* password = PW_ME;
unsigned long lastTimeHost = 0;
unsigned long lastTimeRefresh = 0;
int NUM_LEDS=    MAX_LEDS;

int brightness = BRIGHTNESS;

int noise = 0;
int threshold = 80;

uint8_t gCurrentPatternNumber = 2;

// Each row contains the LED numbers for that row
// [rows][leds], where [][0] is #leds in this row]
int rowmatrix[ROWS][13] = {
  {5 ,12,26,40,54,68},
  {10,11,13,25,27,39,41,53,55,67,69},
  {11,0 ,10,14,24,28,38,42,52,56,66,70},
  {12,1 ,9 ,15,23,29,37,43,51,57,65,71,79},
  {12,2 ,8 ,16,22,30,36,44,50,58,64,72,78},
  {12,3 ,7 ,17,21,31,35,45,49,59,63,73,77},
  {12,4 ,6 ,18,20,32,34,46,48,60,62,74,76},
  {6 ,5 ,19,33,47,61,75}
};  

// Spitzen {6 ,5 ,19,33,47,61,75}
int spitzen[6] = {5 ,19,33,47,61,75};

// Each tooth contains the LED numbers for that Zacken
// [tooth][leds], where [][0] is #leds in this row]
int toothmatrix[ZACKEN][15] = {
  {13 ,0 ,1 ,2 ,3 ,4,5 ,6 ,7, 8, 9,10,11,12},
  {14,13,14,15,16,17,18,19,20,21,22,23,24,25,26},
  {14,27,28,29,30,31,32,33,34,35,36,37,38,39,40},
  {14,41,42,43,44,45,46,47,48,49,50,51,52,53,54},
  {14,55,56,57,58,59,60,61,62,63,64,65,66,67,68},
  {11,69,70,71,72,73,74,75,76,77,78,79}
};
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN_STRIP, COLOR_ORDER + NEO_KHZ800);


void setSpitzen(bool onoff) {
  int32_t spitzencolor;
  if ( onoff == true ) { 
    spitzencolor = strip.Color(255, 255, 255, 255);
  } else {
    spitzencolor = strip.Color(0, 0, 0, 0);
  }
  for(int j=0; j < ZACKEN; j++) {
    strip.setPixelColor(spitzen[j], spitzencolor);
  }
  strip.show();
}

void setRowColor(int row, int32_t c) {
   for(int j=1; j <= rowmatrix[row][0]; j++) { // iterate over all leds in row. Element 0 contains number of leds in row
      strip.setPixelColor(rowmatrix[row][j], c);
   }  
}

void setZackenColor(int zacken, int32_t c) {
   for(int j=1; j <= toothmatrix[zacken][0]; j++) { // iterate over all leds in tooth. Element 0 contains number of leds in row
      strip.setPixelColor(toothmatrix[zacken][j], c);
   }  
}

void setAll (int32_t c) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    strip.setPixelColor(i, c);
  }
}

void setsound() {
  noise = analogRead(PIN_ANALOG_IN);
  for(int i = 0; i < NUM_LEDS; i++ ) {
      if (noise > threshold) {
         strip.setPixelColor(i, strip.Color(0, 0, 0, 255));
      } else {
         strip.setPixelColor(i, strip.Color(0, 0, 0, 0));
      } 
  }
  strip.show();   
}

void setsound2() {
  byte r,g,b,w;
   for(int i=0; i < ZACKEN; i++) { 
      r = random(200);
      g = random(200);
      b = random(200);
      w = random(200);
      noise = analogRead(PIN_ANALOG_IN);
      if (noise > threshold) {
         setZackenColor(i, strip.Color(r, g, b, w));
      } else {
         setZackenColor(i, strip.Color(0, 0, 0, 0));
      }
      strip.show();
  }    
}

void setsound3() {
  byte r,g,b, w;
  for(int i = 0; i < NUM_LEDS; i++ ) {
      r = (byte) random(0, 100);
      g = (byte) random(0, 100);
      b = (byte) random(0, 100);
      w = (byte) random(0, 50);
      noise = analogRead(PIN_ANALOG_IN);
      if (noise > threshold) {
         strip.setPixelColor(i, strip.Color(noise +r , noise +g, noise +b, 0));
      } else {
         strip.setPixelColor(i, strip.Color(0, 0, 0, 0));
      }
      strip.show();
  }    
}

void fastrowsoundspitzen() {
  for(int i=0; i < ROWS; i++) { // iterate over each row
    for(int j = 0; j < 3; j++ ) {
      noise = analogRead(PIN_ANALOG_IN); 
      if (noise > threshold) {
        setSpitzen(true);
      } else {
        setSpitzen(false);
      }
      switch(j) { 
        case 0: setRowColor(i, strip.Color(255, 0, 0, 0)); break;
        case 1: setRowColor(i, strip.Color(0, 255, 0, 0)); break;
        case 2: setRowColor(i, strip.Color(0, 0, 127, 0)); break;
      }
      strip.show();
      delay(50);
      noise = analogRead(PIN_ANALOG_IN); 
      if (noise > threshold) {
        setSpitzen(true);
      } else {
        setSpitzen(false);
      }
      switch(j) { 
        case 0: setRowColor(i, strip.Color(0, 0, 0, 0)); break;
        case 1: setRowColor(i, strip.Color(0, 0, 0, 0)); break;
        case 2: setRowColor(i, strip.Color(0, 0, 0, 0)); break;
      }
      strip.show();
      delay(50);
    }
  }
}

void RGBLoop(){
  for(int j = 0; j < 3; j++ ) { 
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      switch(j) { 
        case 0: setAll(strip.Color(k, 0, 0, 0)); break;
        case 1: setAll(strip.Color(0, k, 0, 0)); break;
        case 2: setAll(strip.Color(0, 0, k, 0)); break;
      }
      strip.show();
      httpServer.handleClient();
      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 
      switch(j) { 
        case 0: setAll(strip.Color(k, 0, 0, 0)); break;
        case 1: setAll(strip.Color(0, k, 0, 0)); break;
        case 2: setAll(strip.Color(0, 0, k, 0)); break;
      }
      strip.show();
      httpServer.handleClient();
      delay(3);
    }
  }
}

void fastzacken() { 
  for(int i=0; i < ZACKEN; i++) { // iterate over each row
    for(int j = 0; j < 3; j++ ) { 
      switch(j) { 
        case 0: setZackenColor(i, strip.Color(255, 0, 0, 0)); break;
        case 1: setZackenColor(i, strip.Color(0, 255, 0, 0)); break;
        case 2: setZackenColor(i, strip.Color(0, 0, 127, 0)); break;
      }
      strip.show();
      delay(50);
      switch(j) { 
        case 0: setZackenColor(i, strip.Color(0, 0, 0, 0)); break;
        case 1: setZackenColor(i, strip.Color(0, 0, 0, 0)); break;
        case 2: setZackenColor(i, strip.Color(0, 0, 0, 0)); break;
      }
      strip.show();
      delay(50);
    }
  }
}

void slowzacken() {
    for(int i=0; i < ZACKEN; i++) { // iterate over each row
      setZackenColor(i, strip.Color(255, 0, 0));
      strip.show(); // show leds after each row
      httpServer.handleClient();
      delay(500);
      setZackenColor(i, strip.Color(0, 0, 0));
      strip.show(); // show leds after each row
      httpServer.handleClient();
      delay(500);
      setZackenColor(i, strip.Color(0, 255, 0));
      strip.show(); // show leds after each row
      httpServer.handleClient();
      delay(500);
      setZackenColor(i, strip.Color(0, 0, 0));
      strip.show(); // show leds after each row
      httpServer.handleClient();
      delay(500);
    }  
}

void color_chase(uint32_t c, uint8_t wait) {
  // Move a single led
  for(int i = 0; i < NUM_LEDS; i++) {
    // Turn our current led ON, then show the leds
   strip.setPixelColor(i, c);
   strip.show();
   delay(wait);
   strip.setPixelColor(i, strip.Color(0, 0, 0, 0));
   strip.show();
   httpServer.handleClient();
  }
}

void dancingdot()
{
  color_chase(strip.Color(100, 0, 255, 10), 15);
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }
  return c;
}

void rainbowcyclesound() {
  int SpeedDelay = 20;
  byte *c;
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ROWS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      noise = analogRead(PIN_ANALOG_IN);
      if (noise > threshold) {
         setRowColor(i, strip.Color(*c, *(c+1), *(c+2), noise + (byte) random(100)));
      } else {
         setRowColor(i, strip.Color(*c, *(c+1), *(c+2), 0));
      }
      httpServer.handleClient();
    }
   strip.show();
   delay(SpeedDelay);
  }
}

void rainbowcycle() {
  int SpeedDelay = 20;
  byte *c;
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      strip.setPixelColor(i, strip.Color(*c, *(c+1), *(c+2), 0));
      httpServer.handleClient();
    }
   strip.show();
   delay(SpeedDelay);
  }
}

void handleRoot() {
  String tmp = "";
   // data for threshold
  tmp = httpServer.arg("threshold");
  if (tmp != "") { threshold = tmp.toInt();}
  
  // data for pattern
  tmp = httpServer.arg("pattern");
  if (tmp != "") { gCurrentPatternNumber = tmp.toInt();}  
  
  // building a website
  char temp[2048];
 
  snprintf ( temp, 2048,
"<!DOCTYPE html>\n<html>\n\
  <head>\n\
    <title>Kronleuchter</title>\n\
    <style>\
      body { background-color: #cccccc; font-family: Arial; Color: #008; }\
    </style>\n\
    <meta name=\"viewport\" content=\"width=device-width, height=device-height, initial-scale=1.0, user-scalable=0, minimum-scale=1.0, maximum-scale=1.0\" />\n\
  </head>\n\
  <body>\n\
    <p><a href=\"/update\">Firmware&nbsp;Update</a></p>\n\
    <h1>Kronleuchter</h1>\n\
    \n\
    <form action=\"/\" name=\"pick\" method=\"post\">\n\
    Noise&nbsp;Threshold:&nbsp;<input type=\"text\" name=\"threshold\" value=\"%02d\" onchange=\"document.forms['pick'].submit();\" /><br/>\n\
    <br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"0\" />Sound1<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"1\" />Sound2<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"2\" />Sound3<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"3\" />FastRowSoundSpitzen<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"4\" />SlowZacken<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"5\" />FastZacken<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"6\" />RGBLoop<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"7\" />DancingDot<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"8\" />RainbowCycle<br/>\n\
    <input type=\"radio\" name=\"pattern\" value=\"9\" />RainbowCycleSound<br/>\n\
    <br/>\n\
    <span onclick=\"document.forms['pick'].submit();\" style=\"font-size:16pt;\">CHANGE </span>\n\
    </form>\n\
   \n\
  </body>\n\
</html>",
  threshold
  );
  httpServer.send ( 200, "text/html", temp );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += ( httpServer.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";

  for ( uint8_t i = 0; i < httpServer.args(); i++ ) {
    message += " " + httpServer.argName ( i ) + ": " + httpServer.arg ( i ) + "\n";
  }

  httpServer.send ( 404, "text/plain", message );
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { setsound, setsound2, setsound3, fastrowsoundspitzen, 
                                slowzacken, fastzacken, RGBLoop, dancingdot, rainbowcycle, 
                                rainbowcyclesound };

// Wifi Connection initialisieren
void WifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(800);
  }
}

// HTTP updater initialisieren
void HTTPUpdateConnect() 
{
   httpUpdater.setup(&httpServer, "/update");    //, "admin", "fnord");
   httpServer.begin();
}

void setup() {
   //randomSeed(analogRead(PIN_ANALOG_IN));
   randomSeed(5345);
   Serial.begin(9600);
   Serial.println("Kronleuchter"); Serial.println("");
    
   delay(3000); // 3 second delay for recovery (Stand irgendwo, dass das gut ist)
   // Initialize Neopixel Strip
   strip.setBrightness(brightness);
   strip.begin();
   strip.show(); 
   delay(50);
   Serial.println("NeoPixel started");
   // Wifimanager iniitaliaisern. Bucht sich in ein bekanntes Netz ein. Wenn er kein bekanntes Netz findet macht er einen Hostspot auf, um den client zu konfigurieren.
   WiFiManager wifiManager;
   WiFi.hostname(host);
   MDNS.begin(host);
   MDNS.addService("http", "tcp", 80);
   // Hotspot Name um den Client zu konfigurieren, wenn er kein bekanntes Netz findet
   wifiManager.autoConnect("kronleuchter-config");
   // Initialisiere das Filesstem
   // HTTP Server für Firmware updates http://<ip>/update
   httpUpdater.setup(&httpServer, "/update", "admin", "fnord");
   httpServer.on ( "/", handleRoot );
   httpServer.onNotFound ( handleNotFound );
   httpServer.begin();
   Serial.println("Initialized http /update server");
   // HTTP Server für die Steuerung
   // HTTPServerInit();
   Serial.println("Initialized http server");
   // rote Lampe wenn boot komplett.
   pinMode(LED_BUILTIN, OUTPUT);
   Serial.println("Red led finishes init phase");
}

void loop() {
   // Muss der Webserver auf etwas antworten?
   if (millis() - lastTimeHost > 10)
   {  
      httpServer.handleClient();
      lastTimeHost = millis();
    }
    //Aktuelles Patternmuster ausführen
    gPatterns[gCurrentPatternNumber](); 
}

