#include <Button.h>
#include <FastLED.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


SoftwareSerial softSerial(/*rx =*/D5, /*tx =*/D6); // has to be inverted
#define FPSerial softSerial

#define NUM_LEDS 37

#define DATA_PIN D7  // DHD leds

#define BUTTON_PIN D2 // menu control

// Replace with your network credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Start a web server on port 80 (standard HTTP port)
ESP8266WebServer server(80);

// Pin definition (using the built-in LED on the D1 Mini)
const int ledPin = LED_BUILTIN; 

// HTML code for the webpage (stored in flash memory)
const char HTML[] PROGMEM =  R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Stargate DHD</title>
    <style>
        body { font-family: 'Segoe UI', Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f0f2f5; color: #333; }
        .container { max-width: 400px; margin: 0 auto; background: white; padding: 30px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.1); }
        .button { background-color: #007bff; border: none; color: white; padding: 14px 28px; 
                  text-align: center; font-size: 16px; margin: 10px; cursor: pointer; 
                  border-radius: 6px; width: 80%; transition: background 0.2s; font-weight: bold; }
        .button:hover { background-color: #0056b3; }
        .button-off { background-color: #dc3545; }
        .button-off:hover { background-color: #bd2130; }
        #status-msg { margin-top: 20px; font-weight: bold; color: #28a745; min-height: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Control your Stargate DHD</h2>
        <p>Control your device seamlessly.</p>
        
        <!-- Buttons call JavaScript functions instead of normal links -->
        <button class="button" onclick="sendCommand('/handleDialing')">Random Dial</button>

        <button class="button" onclick="sendCommand('/handleEarth')">Call Earth</button>
        <button class="button" onclick="sendCommand('/handleAbydos')">Call Abydos</button>
        <button class="button" onclick="sendCommand('/handleChulak')">Call Chulak</button>

        <button class="button button-off" onclick="sendCommand('/led-off')">Turn Off LED</button>
        <button class="button button-off" onclick="sendCommand('/led-on')">Turn On LED</button>

        <div id="status-msg"></div>
    </div>

    <script>
    // This function sends a stealth request to the D1 Mini in the background
    function sendCommand(endpoint) {
        let msgDiv = document.getElementById("status-msg");
        
        fetch(endpoint)
            .then(response => {
                if(response.ok) {
                    msgDiv.innerText = "Command executed successfully!";
                    msgDiv.style.color = "#28a745";
                } else {
                    msgDiv.innerText = "Server error occurred.";
                    msgDiv.style.color = "#dc3545";
                }
                // Clear the status message after 2 seconds
                setTimeout(() => { msgDiv.innerText = ""; }, 2000);
            })
            .catch(err => {
                msgDiv.innerText = "Failed to connect to D1 Mini.";
                msgDiv.style.color = "#dc3545";
            });
    }
    </script>
</body>
</html>
)=====";

// This function runs when you access the root IP address
void handleRoot() {
  server.send(200, "text/html", HTML);
}

// CRGB dialingLed[NUM_LEDS];
CRGBArray<NUM_LEDS> dialingLed;

// The Outer Ring (19 Keys, Clockwise)
CRGBSet Crater(dialingLed(0, 0)); // 1. Crater (Bowl)
CRGBSet Virgo(dialingLed(1, 1)); // 2. Virgo
CRGBSet dhd(dialingLed(1, 2)); // 3. Boötes
CRGBSet Centaurus(dialingLed(0, 1)); // 4. Centaurus
CRGBSet Libra(dialingLed(0, 1)); // 5. Libra
CRGBSet SerpensCaput(dialingLed(0, 1)); // 6. Serpens Caput
CRGBSet Norma(dialingLed(0, 1)); // 7. Norma
CRGBSet Scorpius(dialingLed(0, 1)); // 8. Scorpius
CRGBSet CoronaAustralis(dialingLed(0, 1)); // 9. Corona Australis (Cra)
CRGBSet Scutum(dialingLed(0, 1)); // 10. Scutum
CRGBSet Sagittarius(dialingLed(0, 1)); // 11. Sagittarius
CRGBSet Aquila(dialingLed(0, 1)); // 12. Aquila
CRGBSet Microscopium(dialingLed(0, 1)); // 13. Microscopium (Mic)
CRGBSet Capricornus(dialingLed(0, 1)); // 14. Capricornus
CRGBSet PiscisAustrinus(dialingLed(0, 1)); // 15. Piscis Austrinus
CRGBSet Equuleus(dialingLed(0, 1)); // 16. Equuleus
CRGBSet Aquarius(dialingLed(0, 1)); // 17. Aquarius
CRGBSet Pegasus(dialingLed(0, 1)); // 18. Pegasus
CRGBSet Sculptor(dialingLed(0, 1)); // 19. Sculptor

// The Inner Ring (19 Keys, Clockwise)
// Starting again from the absolute top (12 o'clock position, directly below Crater) and moving clockwise:
CRGBSet Pisces(dialingLed(0, 1)); // 1. Pisces
CRGBSet Andromeda(dialingLed(0, 1)); // 2. Andromeda
CRGBSet Triangulum(dialingLed(0, 1)); // 3. Triangulum
CRGBSet Aries(dialingLed(0, 1)); // 4. Aries
CRGBSet Perseus(dialingLed(0, 1)); // 5. Perseus
CRGBSet Cetus(dialingLed(0, 1)); // 6. Cetus
CRGBSet Taurus(dialingLed(0, 1)); // 7. Taurus
CRGBSet Auriga(dialingLed(0, 1)); // 8. Auriga
CRGBSet Eridanus(dialingLed(0, 1)); // 9. Eridanus
CRGBSet Orion(dialingLed(0, 1)); // 10. Orion
CRGBSet CanisMinor(dialingLed(0, 1)); // 11. Canis Minor
CRGBSet Monoceros(dialingLed(0, 1)); // 12. Monoceros
CRGBSet Gemini(dialingLed(0, 1)); // 13. Gemini
CRGBSet Hydra(dialingLed(0, 1)); // 14. Hydra
CRGBSet Lynx(dialingLed(0, 1)); // 15. Lynx
CRGBSet Cancer(dialingLed(0, 1)); // 16. Cancer
CRGBSet Sextans(dialingLed(0, 1)); // 17. Sextans
CRGBSet LeoMinor(dialingLed(0, 1)); // 18. Leo Minor
CRGBSet Leo(dialingLed(0, 1)); // 19. Leo

Button button(BUTTON_PIN);

DFRobotDFPlayerMini myDFPlayer;

int startMillis = 0;
int lockChevronMillis = 0;
bool lockChevronOn = false;
int lockChevronFade = 0;
int lockedChevronFade = 0;
bool dialing = false;
bool dhdActive = false;
bool chevronLocked = false;
bool finalLock = false;
int buttonPressCount = 0;

void setup()
{
  initPlayer();

  Serial.begin(9600);

  button.begin();

  FastLED.addLeds<WS2812B, DATA_PIN, BGR>(dialingLed, NUM_LEDS);
  // set_max_power_in_volts_and_milliamps(5, 500); // FastLED Power management set at 5V, 500mA.
  FastLED.setBrightness(240); // out of 255

  initWifiAndServer();
}

void initWifiAndServer()
{
 // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print the D1 Mini's IP address
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

    // Define the URI routes and associate them with functions
  server.on("/", handleRoot);
  server.on("/handleDialing", handleDialing);
  server.on("/handleDemo", handleDemo);
  server.on("/handleTheme", handleTheme);

  server.on("/handleEarth", handleEarth);
  server.on("/handleAbydos", handleAbydos);
  server.on("/handleChulak", handleChulak);

  server.on("/led-off", handleLedOff);
  server.on("/led-on", handleLedOn);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
 } 

void initPlayer()
{
  FPSerial.begin(9600);

  Serial.println();
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true))
  { // Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.stop();
  // myDFPlayer.enableLoop();
}

void loop()
{
  // Continuously listen for incoming webpage clients
  server.handleClient();

  if(button.pressed()){
    buttonPressCount++;
    if (buttonPressCount > 4) { buttonPressCount = 0; } // increase to 3 if you want to play the theme without sound

    FastLED.clear();
    FastLED.show();
    // Stop playing MP3
    myDFPlayer.stop();
    startMillis = millis();

    // reset to default
    for (int i = 0; i <= NUM_LEDS2; i++) { dhd[i][0] = 0; dhd[i][1] = 255; dhd[i][2] = 1; dhd[i][3] = 0; }

    if (buttonPressCount == 1) {
      handleDialing();
    }
    else if (buttonPressCount == 2) {
      handleDemo();
    }
    else if (buttonPressCount == 3) {
      handleTheme();
    }
    else {
      Serial.println(F("No Button, stop all"));
      dialing = false;
      myDFPlayer.stop();
      FastLED.clear();
    }
  }

  if (dialing){
    dialingSequence();
  }

  FastLED.show();
}

void handleLedOn() {
  digitalWrite(ledPin, LOW); // Turn on built-in LED

  server.send(200, "text/plain", "OK");
}

void handleLedOff() {
  digitalWrite(ledPin, HIGH); // Turn off built-in LED
  
  server.send(200, "text/plain", "OK");
}

void handleDialing() {
  // How long dialing sequence has been running since button press
  long dialDuration = millis() - startMillis;

  Serial.println(F("1. Dialing"));
  myDFPlayer.stop();
  dialing = true;
  myDFPlayer.volume(19);
  myDFPlayer.play(1);

  if (dialDuration > 67000) {
    dialing = false;
  }

  server.send(200, "text/plain", "OK");
}

void handleDemo() {
  Serial.println(F("2. Demo"));
  myDFPlayer.stop();
  dialing = false;
  //myDFPlayer.volume(10);
  //myDFPlayer.loop(2);
 
  // WIP

  server.send(200, "text/plain", "OK");
}

void handleTheme() {
  Serial.println(F("3. Play theme"));
  myDFPlayer.stop();
  myDFPlayer.volume(19);
  myDFPlayer.play(1);
  dialing = false;

  // WIP

  server.send(200, "text/plain", "OK");
}

// Earth (Tau'ri)	Auriga ➔ Cetus ➔ Centaurus ➔ Cancer ➔ Scutum ➔ Eridanus
void handleEarth() {
  // vibrant, high-brightness Orange.
  // Hue (H)	19	The color type. On FastLED's 0-255 scale, 0 is Red and 32 is Orange. 19 sits between them, resulting in a reddish-orange or "Amber" tone.
  // Saturation (S)	255	The intensity. 255 is "maximum saturation," meaning the color is pure and has no white mixed in.
  // Value (V)	255	The brightness. 255 is the maximum brightness setting for that specific pixel.

  Auriga = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Cetus = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Centaurus = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Cancer = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Scutum = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Eridanus = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
}

// Abydos (Mined by Ra)	Taurus ➔ Serpens Caput ➔ Capricornus ➔ Monoceros ➔ Sagittarius ➔ Orion
void handleAbydos() {
  Taurus = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  SerpensCaput = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Capricornus = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Monoceros = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Sagittarius = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Orion = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
}

// Chulak (Teal'c's home)	Capricornus ➔ Cetus ➔ Scorpio ➔ Pisces ➔ Gemini ➔ Leo
void handleChulak() {
  Capricornus = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Cetus = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Scorpio = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Pisces = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Gemini = CHSV(19,255,255);
  FastLED.show();
  delay(2000);

  Leo = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
}


  
  // Praclarush Taonas (Lost in fire)	Orion ➔ Boötes ➔ Piscis Austrinus ➔ Leo Minor ➔ Perseus ➔ Sextans
  // Tollan (Destroyed homeworld)	Aries ➔ Taurus ➔ Gemini ➔ Cancer ➔ Leo ➔ Virgo
  // Tollana (New Tollan home)	Andromeda ➔ Aquarius ➔ Aries ➔ Corona Australis ➔ Leo Minor ➔ Pisces
  // K'Tau (Asgard protected)	Aquila ➔ Crater ➔ Orion ➔ Sagittarius ➔ Cetus ➔ Gemini
  // Tartarus (Anubis's drone base)	Pisces ➔ Virgo ➔ Capricornus ➔ Aries ➔ Sagittarius ➔ Leo
  // Vagon Brei (Morgan le Fay's world)	Boötes ➔ Orion ➔ Andromeda ➔ Aquila ➔ Microcopium ➔ Leo Minor
  // P3W-451 (The Black Hole planet)	Capicornus ➔ Taurus ➔ Scorpio ➔ Aries ➔ Virgo ➔ Leo
  // P3X-562 (Living blue crystals)	Boötes ➔ Cancer ➔ Virgo ➔ Leo ➔ Taurus ➔ Capricornus
  // PB5-926 (Desert world / Sokar)	Sagittarius ➔ Cancer ➔ Triangulum ➔ Auriga ➔ Serpens Caput ➔ Taurus
  // PX1-767 (Camelot)	Orion ➔ Andromeda ➔ Leo Minor ➔ Orion ➔ Pisces ➔ Capricornus
}

