#include <Button.h>
#include <FastLED.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "webpage.h" // HTML code for the webpage (stored in flash memory)

SoftwareSerial softSerial(/*rx =*/D5, /*tx =*/D6); // has to be inverted 
#define FPSerial softSerial

#define NUM_LEDS 50
#define NUM_LEDS2 47

#define DATA_PIN D7 // Dialing
#define DATA_PIN2 D8 // Wormhole

#define BUTTON_PIN D2

// Replace with your network credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Start a web server on port 80 (standard HTTP port)
ESP8266WebServer server(80);

// Pin definition (using the built-in LED on the D1 Mini)
const int ledPin = LED_BUILTIN; 

// HTML code for the webpage (stored in flash memory)


// This function runs when you access the root IP address
void handleRoot() {
  server.send(200, "text/html", HTML);
}

//CRGB dialingLed[NUM_LEDS];
CRGBArray<NUM_LEDS> dialingLed;
CRGBSet chevron05(dialingLed(0,2)); // 0+1+2 = 3 leds
CRGBSet chevron06(dialingLed(6,8)); // 6+7+8
CRGBSet chevron07(dialingLed(12,14)); // 12,13,14
CRGBSet chevron08(dialingLed(18,20)); // 18,19,20
CRGBSet lockingChevron(dialingLed(25,29)); // 25,26,27,28,29
CRGBSet chevron01(dialingLed(30,32)); // 30,31,32
CRGBSet chevron02(dialingLed(36,38)); // 36,37,38
CRGBSet chevron03(dialingLed(42,44)); // 42,43,44
CRGBSet chevron04(dialingLed(47,49)); // 47,48,49

CRGBArray<NUM_LEDS2> wormHoleLed;

Button button(BUTTON_PIN);

DFRobotDFPlayerMini myDFPlayer;

int startMillis = 0;
int lockChevronMillis = 0;
bool lockChevronOn = false;
int lockChevronFade = 0;
int lockedChevronFade = 0;
bool dialing = false;
bool wormholeActive = false;
int wormholeStartMillis = 0;
int wormholeBrightness = 0;
bool chevronLocked = false;
bool finalLock = false;
int buttonPressCount = 0;

// {Active, Brightness, FadeDirection, Color}
int wormholeArray[NUM_LEDS2][4] = {{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0},{0,255,1,0}};


void setup()
{
  initPlayer();

  Serial.begin(9600);
  
  button.begin();

  // Most modern addressable LEDs (like WS2812B or SK6812) don't actually receive colors in "Red, Green, Blue" order. Many are manufactured as GRB (Green-Red-Blue) or BRG.
  // You don't need to change the CHSV code!. You just need to tell the library FastLED.addLeds() how your specific hardware is wired in this void setup() section of the code.

  // How to Test Your Order
  // You can run a simple test to find your exact order. Run the chevron demo, it should produce an orange color but:
  // If the LED turns Green, your order starts with G.
  // If the LED turns Blue, your order starts with B.
  // If the LED turns Red, your order starts with R.

  // I put the all possible combination below:
  // RGB
  // RBG
  // GRB (Most common for WS2812B)
  // GBR
  // BRG
  // BGR

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(dialingLed, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN2, RGB>(wormHoleLed, NUM_LEDS2);

  //set_max_power_in_volts_and_milliamps(5, 500); // FastLED Power management set at 5V, 500mA.
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
  server.on("/handlePlayTheme", handlePlayTheme);
  server.on("/handleChevronDemo", handleChevronDemo);
  server.on("/handleChevronDemo", handleStopAll);
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
  
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true){
        delay(0); // Code to compatible with ESP8266 watch dog.
      }
    }
    Serial.println(F("DFPlayer Mini online."));

    myDFPlayer.stop();
    //myDFPlayer.enableLoop();
}

void loop()
{
  if(button.pressed()){
    buttonPressCount++;
    if (buttonPressCount > 4) { buttonPressCount = 0; } // increase to 3 if you want to play the theme without sound

    FastLED.clear();
    FastLED.show();
    // Stop playing MP3
    myDFPlayer.stop();
    startMillis = millis();
    wormholeActive = false;

    // reset to default
    for (int i = 0; i <= NUM_LEDS2; i++) { wormholeArray[i][0] = 0; wormholeArray[i][1] = 255; wormholeArray[i][2] = 1; wormholeArray[i][3] = 0; }

    if (buttonPressCount == 1) {
      handleDialing();
    }
    else if (buttonPressCount == 2) {
      handleDemo();
    }
    else if (buttonPressCount == 3) {
      handlePlayTheme();
    }
    else if (buttonPressCount == 4) {
      handleChevronDemo() ;
    }
    else {
      handleStopAll();
    }
  }

  if (dialing){
    dialingSequence();
  }

  if (wormholeActive){
    wormholeSequence();
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
  Serial.println(F("1. Dialing"));
  myDFPlayer.stop();
  dialing = true;
  wormholeActive = false;
  myDFPlayer.volume(19);
  myDFPlayer.play(1);

  dialingSequence();
  FastLED.show();

  server.send(200, "text/plain", "OK");
}

void handleDemo() {
  Serial.println(F("2. Demo"));
  myDFPlayer.stop();
  dialing = false;
  wormholeActive = true;
  //myDFPlayer.volume(10);
  //myDFPlayer.loop(2);
  wormholeDemo();
  
  wormholeSequence();
  FastLED.show();

  server.send(200, "text/plain", "OK");
}

void handlePlayTheme() {
  Serial.println(F("3. Play theme"));
  myDFPlayer.stop();
  myDFPlayer.volume(19);
  myDFPlayer.play(1);
  dialing = false;
  wormholeActive = false;

  FastLED.show();

  server.send(200, "text/plain", "OK");
}

void handleChevronDemo() {
  Serial.println(F("4. Chevron demo"));
  myDFPlayer.stop();
  dialing = false;
  wormholeActive = false;
  chevronDemo();

  FastLED.show();

  server.send(200, "text/plain", "OK");
}

void handleStopAll() {
  Serial.println(F("No Button, stop all"));
  wormholeActive = false;
  dialing = false;
  myDFPlayer.stop();
  FastLED.clear();

  FastLED.show();

  server.send(200, "text/plain", "OK");
}

void chevronDemo() {
  // vibrant, high-brightness Orange.
  // Hue (H)	19	The color type. On FastLED's 0-255 scale, 0 is Red and 32 is Orange. 19 sits between them, resulting in a reddish-orange or "Amber" tone.
  // Saturation (S)	255	The intensity. 255 is "maximum saturation," meaning the color is pure and has no white mixed in.
  // Value (V)	255	The brightness. 255 is the maximum brightness setting for that specific pixel.
  chevron01 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  chevron02 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  chevron03 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  chevron04 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  chevron05 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  chevron06 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  chevron07 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  chevron08 = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
  lockingChevron = CHSV(19,255,255);
  FastLED.show();
  delay(2000);
}

void wormholeDemo() {
  //wormholeStartMillis = millis();
  if (wormholeActive) {
    // vibrant, high-brightness Orange.
    // Hue (H)	19	The color type. On FastLED's 0-255 scale, 0 is Red and 32 is Orange. 19 sits between them, resulting in a reddish-orange or "Amber" tone.
    // Saturation (S)	255	The intensity. 255 is "maximum saturation," meaning the color is pure and has no white mixed in.
    // Value (V)	255	The brightness. 255 is the maximum brightness setting for that specific pixel.
    chevron01 = CHSV(19,255,255);
    chevron02 = CHSV(19,255,255);
    chevron03 = CHSV(19,255,255);
    chevron04 = CHSV(19,255,255);
    chevron05 = CHSV(19,255,255);
    chevron06 = CHSV(19,255,255);
    chevron07 = CHSV(19,255,255);
    chevron08 = CHSV(19,255,255);
    lockingChevron = CHSV(19,255,255);
  }
}

void wormholeSequence() {
  long wormholeDuration = millis() - wormholeStartMillis;
  // Bright white, then puddle

  if (wormholeDuration < 1700) {
    wormholeBrightness++;
    if (wormholeBrightness > 210) 
    { 
      wormholeBrightness = 210; 
    }
    for (int i = 0; i < NUM_LEDS2; i++) {
      wormHoleLed[i] = CRGB(wormholeBrightness, wormholeBrightness, wormholeBrightness);
    }
  }
  else {
    EVERY_N_MILLISECONDS(10) {
      wormholeBrightness = 0;
      int randomLED = random8(0, NUM_LEDS2);
      int randomColor = random8(0,3);
      if (randomColor == 0) {
        randomColor = 100; // white?
      }
      else if (randomColor == 1) {
        randomColor = 130; // aqua
      }
      else {
        randomColor = 160; // blue
      }
      if (wormholeArray[randomLED][0] == 0) {
        wormholeArray[randomLED][0] = 1;
        wormholeArray[randomLED][3] = randomColor;
      }
    }

    for (int i = 0; i <= NUM_LEDS2; i++) {
      // If the LED is active, start or continue the fade in/out sequence.
      if (wormholeArray[i][0] == 1) {
        // Increment the brightness either up or down
        wormholeArray[i][1] = wormholeArray[i][1] + (1 * wormholeArray[i][2]);

        // Once full brightness reached, reverse direction.
        if (wormholeArray[i][1] >= 255) {
          wormholeArray[i][1] = 255;
          wormholeArray[i][2] = -wormholeArray[i][2];
        }

        // Once brightness fades back to zero, reset LED to inactive.
        if (wormholeArray[i][1] <= 0) {
          wormholeArray[i][0] = 0;
          wormholeArray[i][1] = 0;
          wormholeArray[i][2] = 1;
        }

        // Set each LED to blue, full saturation, brightness from loop.
        if (wormholeArray[i][3] == 100) { // color is white
          wormHoleLed[i] = CHSV(wormholeArray[i][3], 0, wormholeArray[i][1]);
        }
        else {
          wormHoleLed[i] = CHSV(wormholeArray[i][3], 255, wormholeArray[i][1]);
        }
      }
    }
  }
}


void dialingSequence() {
  // How long dialing sequence has been running since button press
  long dialDuration = millis() - startMillis;

  if (dialDuration > 4998 && dialDuration < 5003) {
    lockedChevronFade = 0;
    chevronLocked = false;
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 11998 && dialDuration < 12003) {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 18998 && dialDuration < 19003) {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 25998 && dialDuration < 26003) {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 32998 && dialDuration < 33003) {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 39998 && dialDuration < 40003) {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 46998 && dialDuration < 47003) {
    setLockingChevronLEDs(true);
  }

  if (lockChevronOn) {
    lockingChevronLEDs();
  }

  if (dialDuration > 6198 && dialDuration < 7203) {
    lockedChevronLEDs(chevron01);
  }
  if (dialDuration > 10000 && dialDuration < 10005) {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 13198 && dialDuration < 14203) {
    lockedChevronLEDs(chevron02);
  }
    if (dialDuration > 20000 && dialDuration < 20005) {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 20198 && dialDuration < 21203) {
    lockedChevronLEDs(chevron03);
  }
    if (dialDuration > 27000 && dialDuration < 27005) {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 27198 && dialDuration < 28203) {
    lockedChevronLEDs(chevron06);
  }
    if (dialDuration > 33000 && dialDuration < 33005) {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 34198 && dialDuration < 35203) {
    lockedChevronLEDs(chevron07);
  }
    if (dialDuration > 40000 && dialDuration < 40005) {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 41198 && dialDuration < 42203) {
    lockedChevronLEDs(chevron08);
  }
  if (dialDuration > 44000 && dialDuration < 44005) {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 48200 && dialDuration < 49205) {
    lockedChevronLEDs(chevron04);
    lockedChevronLEDs(chevron05);
  }

  if (dialDuration > 50400 && dialDuration < 50405) {
    wormholeActive = true;
    wormholeStartMillis = millis();
  }

  if (dialDuration > 66000) {
    wormholeActive = false;
    lockChevronOn = false;
    fadeToBlackBy(dialingLed, NUM_LEDS, 10);
    fadeToBlackBy(wormHoleLed, NUM_LEDS2, 10);
  }

  if (dialDuration > 67000) {
    dialing = false;
  }
}

// Reset variables for the next locking chevron run
void setLockingChevronLEDs(bool finalCall) {
  lockChevronOn = true;
  lockChevronMillis = millis();
  lockChevronFade = 0;
  finalLock = finalCall;
}

// Sets the locking chevron LEDs on, slow fade up and down
void lockingChevronLEDs() {
  // 2 seconds for top chevron to fade in, stay on, then fade out (.5, 1, .5 seconds)
  int lockingChevronDuration = millis() - lockChevronMillis;

  if (lockingChevronDuration < 500) {
    lockChevronFade = lockChevronFade + 2;
  }
  if (lockingChevronDuration > 1500 && !finalLock) {
    lockChevronFade = lockChevronFade - 2;
  }

  if (lockChevronFade > 255) {
    lockChevronFade = 255;
  }
  if (lockChevronFade < 0) {
    lockChevronFade = 0;
  }
  
  lockingChevron = CHSV(19,255,lockChevronFade);

  if (lockingChevronDuration > 2000 && !finalLock) {
    lockChevronOn = false;
    lockingChevron = CHSV(19,255,0);
  }
}

// Sets locked chevron LEDs on after slow fade up
void lockedChevronLEDs(CRGBSet chevronNum) {
  lockedChevronFade = lockedChevronFade + 2;

  if (lockedChevronFade > 255) {
    // vibrant, high-brightness Orange.
    // Hue (H)	19	The color type. On FastLED's 0-255 scale, 0 is Red and 32 is Orange. 19 sits between them, resulting in a reddish-orange or "Amber" tone.
    // Saturation (S)	255	The intensity. 255 is "maximum saturation," meaning the color is pure and has no white mixed in.
    // Value (V)	255	The brightness. 255 is the maximum brightness setting for that specific pixel.
    chevronNum = CHSV(19,255,255); 
    chevronLocked = true;
  }

  if (!chevronLocked) {
    chevronNum = CHSV(19,255,lockedChevronFade);
  }
}
