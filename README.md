# kronleuchter
Wieder ein LED Hut. Diesmal als Krone mit RGBW LEDs und Reaktion auf Lautstärke

Wenn der NodeMCU Saft kriegt und das zuletzt eingestellte WLAN nicht erreichbar ist, macht er den AP kronleuchter-config auf.
Mit diesem AP verbindet man sich und sollte autmatisch uf das captive portal kommen, wo man sich ein WLAN aussuchen kann.

Nach dem Neustart meldet sich der NodeMCU im WLAN an und sollte unter http://kroleuchter.local errichbar sein.
Updates kann man unter http://kroleuchter.local/update hochladen. (Arduino IDE -> Sketch -> Export compiled binary).

Abhängigkeiten Arduino IDE:
File -> Preferences -> Additional Board Manager URLs -> http://arduino.esp8266.com/stable/package_esp8266com_index.json
Board: NodeMCU 1.0 (ESP 12E Module)
Libraries:
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h> 
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>


Krone (RGBW 60/m) /\/\/\

PIN Belegung
Wemos NodeMCU mini 						LED Strip
D4					<->					Data LED
G					<-> GND USB 	<-> GND LED
5V					<-> 5V USB 		<-> VCC LED

Sound Sensor V2		NodeMCU mini
AOUT			<->	A0
VCC 			<->	3.3V
GND				<->	GND

