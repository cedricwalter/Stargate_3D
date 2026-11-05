#include <Button.h>
#include <FastLED.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "webpage.h" // HTML code for the webpage (stored in flash memory)

SoftwareSerial softSerial(/*rx =*/D5, /*tx =*/D6);  // has to be inverted
#define FPSerial softSerial

#define NUM_LEDS 37

#define DATA_PIN D7  // DHD leds

#define BUTTON_PIN D2  // menu control

// Replace with your network credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Start a web server on port 80 (standard HTTP port)
ESP8266WebServer server(80);

// Pin definition (using the built-in LED on the D1 Mini)
const int ledPin = LED_BUILTIN;

// This function runs when you access the root IP address
void handleRoot() {
  server.send(200, "text/html", HTML);
}

// CRGB dialingLed[NUM_LEDS];
CRGBArray<NUM_LEDS> dialingLed;
CRGBArray<NUM_LEDS> leds; 

// Helper to mimic your custom CRGBSet allocation wrapper
CRGBSet dialingLed2(int start, int end) {
  return leds(start, end);
}
// --- DHD LED VARIABLE ASSIGNMENTS ---
// Explicitly binding indices to your variables.
// Note: Index #0 in frontend = Earth Point of Origin.

// The Outer Ring (19 Keys, Clockwise)
CRGBSet Earth(dialingLed(0, 0));             // #1  Point of Origin
CRGBSet Crater(dialingLed(1, 1));            // #2  Crater
CRGBSet Virgo(dialingLed(2, 2));             // #3  Virgo
CRGBSet dhd(dialingLed(3, 3));               // #4  Boötes (Custom variable)
CRGBSet Centaurus(dialingLed(4, 4));         // #5  Centaurus
CRGBSet Libra(dialingLed(5, 5));             // #6  Libra
CRGBSet Serpens(dialingLed(6, 6));           // #7  Serpens Caput
CRGBSet Norma(dialingLed(7, 7));             // #8  Norma
CRGBSet Scorpius(dialingLed(8, 8));          // #9  Scorpius
CRGBSet CoronaA(dialingLed(9, 9));           // #10 Corona Australis
CRGBSet Scutum(dialingLed(10, 10));          // #11 Scutum
CRGBSet Sagittarius(dialingLed(11, 11));     // #12 Sagittarius
CRGBSet Aquila(dialingLed(12, 12));          // #13 Aquila
CRGBSet Microscopium(dialingLed(13, 13));    // #14 Microscopium
CRGBSet Capricornus(dialingLed(14, 14));     // #15 Capricornus
CRGBSet PiscisAustrinus(dialingLed(15, 15)); // #16 Piscis Austrinus
CRGBSet Equuleus(dialingLed(16, 16));        // #17 Equuleus
CRGBSet Aquarius(dialingLed(17, 17));        // #18 Aquarius
CRGBSet Pegasus(dialingLed(18, 18));         // #19 Pegasus
CRGBSet Sculptor(dialingLed(19, 19));        // #20 Sculptor

// The Inner Ring (19 Keys, Clockwise)
// Starting again from the absolute top (12 o'clock position, directly below Crater) and moving clockwise:
CRGBSet Pisces(dialingLed(20, 20));          // #21 Pisces
CRGBSet Andromeda(dialingLed(21, 21));       // #22 Andromeda
CRGBSet Triangulum(dialingLed(22, 22));      // #23 Triangulum
CRGBSet Aries(dialingLed(23, 23));           // #24 Aries
CRGBSet Perseus(dialingLed(24, 24));         // #25 Perseus
CRGBSet Cetus(dialingLed(25, 25));           // #26 Cetus
CRGBSet Taurus(dialingLed(26, 26));          // #27 Taurus
CRGBSet Auriga(dialingLed(27, 27));          // #28 Auriga
CRGBSet Eridanus(dialingLed(28, 28));        // #29 Eridanus
CRGBSet Orion(dialingLed(29, 29));           // #30 Orion
CRGBSet CanisMinor(dialingLed(30, 30));      // #31 Canis Minor
CRGBSet Monoceros(dialingLed(31, 31));       // #32 Monoceros
CRGBSet Gemini(dialingLed(32, 32));          // #33 Gemini
CRGBSet Hydra(dialingLed(33, 33));           // #34 Hydra
CRGBSet Lynx(dialingLed(34, 34));            // #35 Lynx
CRGBSet Cancer(dialingLed(35, 35));          // #36 Cancer
CRGBSet Sextans(dialingLed(36, 36));          // #37 Sextans
CRGBSet LeoMinor(dialingLed(37, 37));        // #38 Leo Minor
CRGBSet Leo(dialingLed(38, 38));             // #39 Leo



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

void setup() {
  initPlayer();

  Serial.begin(9600);

  button.begin();

  FastLED.addLeds<WS2812B, DATA_PIN, BGR>(dialingLed, NUM_LEDS);
  // set_max_power_in_volts_and_milliamps(5, 500); // FastLED Power management set at 5V, 500mA.
  FastLED.setBrightness(240);  // out of 255

  initWifiAndServer();
}

void initWifiAndServer() {
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

  server.on("/dial", handleDialRequest);
  server.on("/disengageDHD", handleDisengageRequest);
  

  // Start the server
  server.begin();
  Serial.println("DHD Control Subsystem Operational.");
}

void initPlayer() {
  FPSerial.begin(9600);

  Serial.println();
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true)) {  // Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.stop();
  // myDFPlayer.enableLoop();
}

void loop() {
  // Continuously listen for incoming webpage clients
  server.handleClient();

  if (button.pressed()) {
    buttonPressCount++;
    if (buttonPressCount > 4) { buttonPressCount = 0; }  // increase to 3 if you want to play the theme without sound

    FastLED.clear();
    FastLED.show();
    // Stop playing MP3
    myDFPlayer.stop();
    startMillis = millis();

    // reset to default
    for (int i = 0; i <= NUM_LEDS; i++) {
      dhd[i][0] = 0;
      dhd[i][1] = 255;
      dhd[i][2] = 1;
      dhd[i][3] = 0;
    }

    if (buttonPressCount == 1) {
      handleDialing();
    } else if (buttonPressCount == 2) {
      handleDemo();
    } else if (buttonPressCount == 3) {
      handleTheme();
    } else {
      Serial.println(F("No Button, stop all"));
      dialing = false;
      myDFPlayer.stop();
      FastLED.clear();
    }
  }

  FastLED.show();
}

// Helper function to illuminate a specific glyph variable by ID
void lightUpGlyphLed(int glyphId) {
  // Clear the strip color buffers first (optional, based on desired lock behavior)
  // FastLED.clear(); 

  // Map the 1-39 IDs from the DHD interface to your respective CRGBSet variables
  switch (glyphId) {
    case 1:  Earth = CRGB::OrangeRed; break;
    case 2:  Crater = CRGB::OrangeRed; break;
    case 3:  Virgo = CRGB::OrangeRed;  break;
    case 4:  dhd = CRGB::OrangeRed;    break; // Boötes
    case 5:  Centaurus = CRGB::OrangeRed; break;
    case 6:  Libra = CRGB::OrangeRed;  break;
    
    // Add additional structural cases for IDs 7 through 39 here:
    // case 7: Serpens = CRGB::OrangeRed; break;
    
    default:
      // Fallback for symbols without an explicit variable map yet
      if (glyphId - 1 < NUM_LEDS) {
        leds[glyphId - 1] = CRGB::OrangeRed;
      }
      break;
  }
  
  FastLED.show();
}

void handleDialRequest() {
  if (!server.hasArg("sequence")) {
    server.send(400, "text/plain", "Missing dialing sequence data.");
    return;
  }

  String rawSequence = server.arg("sequence");
  Serial.println("\n====================================");
  Serial.print("DHD GATE LOCK INITIATED: ");
  Serial.println(rawSequence);

  int targetGlyphs[7];
  int counter = 0;
  int lastIndex = 0;
  
  // Parse comma-separated array string values
  for (int i = 0; i < rawSequence.length(); i++) {
    if (rawSequence.charAt(i) == ',' || i == rawSequence.length() - 1) {
      String segment = rawSequence.substring(lastIndex, (i == rawSequence.length() - 1) ? i + 1 : i);
      targetGlyphs[counter] = segment.toInt();
      counter++;
      lastIndex = i + 1;
      if(counter >= 7) break; 
    }
  }

  // Clear all lights before starting a new dial sequence
  FastLED.clear();
  FastLED.show();

  // Progressively lock each chevron step-by-step
  for (int i = 0; i < 7; i++) {
    int currentGlyph = targetGlyphs[i];
    
    Serial.print("Chevron ");
    Serial.print(i + 1);
    Serial.print(" Locked -> Encoding Glyph #");
    Serial.println(currentGlyph);
    
    // Fire the hardware illumination routine
    lightUpGlyphLed(currentGlyph);
    
    // Dramatic delay pacing between chevron structural locks
    delay(400); 
  }
  
  Serial.println("KAWOOSH! WORMHOLE ESTABLISHED.");
  Serial.println("====================================");

  server.send(200, "text/plain", "Wormhole Engaged successfully!");
}

void handleDisengageRequest() {
  Serial.println("\nWormhole connection closed via DHD interface.");
  
  // Visual shutdown effect: Fade out LEDs
  for(int b = 128; b >= 0; b -= 8) {
    FastLED.setBrightness(b);
    FastLED.show();
    delay(15);
  }
  
  FastLED.clear();
  FastLED.setBrightness(128); // Reset brightness back to default
  FastLED.show();

  server.send(200, "text/plain", "Wormhole Disengaged. Gate Safe.");
}


void handleLedOn() {
  digitalWrite(ledPin, LOW);  // Turn on built-in LED

  server.send(200, "text/plain", "OK");
}

void handleLedOff() {
  digitalWrite(ledPin, HIGH);  // Turn off built-in LED

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

  Auriga = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Cetus = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Centaurus = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Cancer = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Scutum = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Eridanus = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);
}

// Abydos (Mined by Ra)	Taurus ➔ Serpens Caput ➔ Capricornus ➔ Monoceros ➔ Sagittarius ➔ Orion
void handleAbydos() {
  Taurus = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Serpens = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Capricornus = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Monoceros = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Sagittarius = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Orion = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);
}

// Chulak (Teal'c's home)	Capricornus ➔ Cetus ➔ Scorpio ➔ Pisces ➔ Gemini ➔ Leo
void handleChulak() {
  Capricornus = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Cetus = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Scorpius = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Pisces = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Gemini = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);

  Leo = CHSV(19, 255, 255);
  FastLED.show();
  delay(2000);
}



// Praclarush Taonas (Lost in fire)	Orion ➔ Boötes ➔ Piscis Austrinus ➔ Leo Minor ➔ Perseus ➔ Sextans
// Tollan (Destroyed homeworld)	Aries ➔ Taurus ➔ Gemini ➔ Cancer ➔ Leo ➔ Virgo
// Tollana (New Tollan home)	Andromeda ➔ Aquarius ➔ Aries ➔ Corona Australis ➔ Leo Minor ➔ Pisces
// K'Tau (Asgard protected)	Aquila ➔ Crater ➔ Orion ➔ Sagittarius ➔ Cetus ➔ Gemini
// Tartarus (Anubis's drone base)	Pisces ➔ Virgo ➔ Capricornus ➔ Aries ➔ Sagittarius ➔ Leo
// Vagon Brei (Morgan le Fay's world)	Boötes ➔ Orion ➔ Andromeda ➔ Aquila ➔ Microcopium ➔ Leo Minor
// P3W-451 (The Black Hole planet)	Capicornus ➔ Taurus ➔ Scorpius ➔ Aries ➔ Virgo ➔ Leo
// P3X-562 (Living blue crystals)	Boötes ➔ Cancer ➔ Virgo ➔ Leo ➔ Taurus ➔ Capricornus
// PB5-926 (Desert world / Sokar)	Sagittarius ➔ Cancer ➔ Triangulum ➔ Auriga ➔ Serpens Caput ➔ Taurus
// PX1-767 (Camelot)	Orion ➔ Andromeda ➔ Leo Minor ➔ Orion ➔ Pisces ➔ Capricornus
