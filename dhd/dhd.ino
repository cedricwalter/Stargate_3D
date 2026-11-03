#include <Button.h>
#include <FastLED.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

SoftwareSerial softSerial(/*rx =*/D5, /*tx =*/D6); // has to be inverted
#define FPSerial softSerial

#define NUM_LEDS 37

#define DATA_PIN D7  // Dialing

#define BUTTON_PIN D2

// CRGB dialingLed[NUM_LEDS];
CRGBArray<NUM_LEDS> dialingLed;

// The Outer Ring (19 Keys, Clockwise)
CRGBSet dhd(dialingLed(0, 1)); // 1. Crater (Bowl)
CRGBSet dhd(dialingLed(0, 1)); // 2. Virgo
CRGBSet dhd(dialingLed(0, 1)); // 3. Boötes
CRGBSet dhd(dialingLed(0, 1)); // 4. Centaurus
CRGBSet dhd(dialingLed(0, 1)); // 5. Libra
CRGBSet dhd(dialingLed(0, 1)); // 6. Serpens Caput
CRGBSet dhd(dialingLed(0, 1)); // 7. Norma
CRGBSet dhd(dialingLed(0, 1)); // 8. Scorpius
CRGBSet dhd(dialingLed(0, 1)); // 9. Corona Australis (Cra)
CRGBSet dhd(dialingLed(0, 1)); // 10. Scutum
CRGBSet dhd(dialingLed(0, 1)); // 11. Sagittarius
CRGBSet dhd(dialingLed(0, 1)); // 12. Aquila
CRGBSet dhd(dialingLed(0, 1)); // 13. Microscopium (Mic)
CRGBSet dhd(dialingLed(0, 1)); // 14. Capricornus
CRGBSet dhd(dialingLed(0, 1)); // 15. Piscis Austrinus
CRGBSet dhd(dialingLed(0, 1)); // 16. Equuleus
CRGBSet dhd(dialingLed(0, 1)); // 17. Aquarius
CRGBSet dhd(dialingLed(0, 1)); // 18. Pegasus
CRGBSet dhd(dialingLed(0, 1)); // 19. Sculptor

// The Inner Ring (19 Keys, Clockwise)
// Starting again from the absolute top (12 o'clock position, directly below Crater) and moving clockwise:

CRGBSet dhd(dialingLed(0, 1)); // 1. Pisces
CRGBSet dhd(dialingLed(0, 1)); // 2. Andromeda
CRGBSet dhd(dialingLed(0, 1)); // 3. Triangulum
CRGBSet dhd(dialingLed(0, 1)); // 4. Aries
CRGBSet dhd(dialingLed(0, 1)); // 5. Perseus
CRGBSet dhd(dialingLed(0, 1)); // 6. Cetus
CRGBSet dhd(dialingLed(0, 1)); // 7. Taurus
CRGBSet dhd(dialingLed(0, 1)); // 8. Auriga
CRGBSet dhd(dialingLed(0, 1)); // 9. Eridanus
CRGBSet dhd(dialingLed(0, 1)); // 10. Orion
CRGBSet dhd(dialingLed(0, 1)); // 11. Canis Minor
CRGBSet dhd(dialingLed(0, 1)); // 12. Monoceros
CRGBSet dhd(dialingLed(0, 1)); // 13. Gemini
CRGBSet dhd(dialingLed(0, 1)); // 14. Hydra
CRGBSet dhd(dialingLed(0, 1)); // 15. Lynx
CRGBSet dhd(dialingLed(0, 1)); // 16. Cancer
CRGBSet dhd(dialingLed(0, 1)); // 17. Sextans
CRGBSet dhd(dialingLed(0, 1)); // 18. Leo Minor
CRGBSet dhd(dialingLed(0, 1)); // 19. Leo


Button button(BUTTON_PIN);

DFRobotDFPlayerMini myDFPlayer;

int startMillis = 0;
int lockChevronMillis = 0;
bool lockChevronOn = false;
int lockChevronFade = 0;
int lockedChevronFade = 0;
bool dialing = false;
bool dhdActive = false;
int wormholeStartMillis = 0;
int wormholeBrightness = 0;
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
  if(button.pressed()){
    buttonPressCount++;
    if (buttonPressCount > 4) { buttonPressCount = 0; } // increase to 3 if you want to play the theme without sound

    FastLED.clear();
    FastLED.show();
    // Stop playing MP3
    myDFPlayer.stop();
    startMillis = millis();

    // reset to default
    for (int i = 0; i <= NUM_LEDS2; i++) { wormholeArray[i][0] = 0; wormholeArray[i][1] = 255; wormholeArray[i][2] = 1; wormholeArray[i][3] = 0; }

    if (buttonPressCount == 1) {
      Serial.println(F("1. Dialing"));
      myDFPlayer.stop();
      dialing = true;
      myDFPlayer.volume(19);
      myDFPlayer.play(1);
    }
    else if (buttonPressCount == 2) {
      Serial.println(F("2. Demo"));
      myDFPlayer.stop();
      dialing = false;
      //myDFPlayer.volume(10);
      //myDFPlayer.loop(2);
      wormholeDemo();
    }
    else if (buttonPressCount == 3) {
      Serial.println(F("3. Play theme"));
      myDFPlayer.stop();
      myDFPlayer.volume(19);
      myDFPlayer.play(1);
      dialing = false;
    }
    else if (buttonPressCount == 4) {
      Serial.println(F("4. DHD demo"));
      myDFPlayer.stop();
      dialing = false;
      chevronDemo();
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

void dialingSequence() {
  // How long dialing sequence has been running since button press
  long dialDuration = millis() - startMillis;

  // WIP

  if (dialDuration > 67000) {
    dialing = false;
  }
}
