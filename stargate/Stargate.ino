#include <Button.h>
#include <FastLED.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

SoftwareSerial softSerial(/*rx =*/D5, /*tx =*/D6); // has to be inverted
#define FPSerial softSerial

#define NUM_LEDS 50
#define NUM_LEDS2 47

#define DATA_PIN D7  // Dialing
#define DATA_PIN2 D8 // Wormhole

#define BUTTON_PIN D2

// CRGB dialingLed[NUM_LEDS];
CRGBArray<NUM_LEDS> dialingLed;
CRGBSet chevron05(dialingLed(0, 2));        // done
CRGBSet chevron06(dialingLed(6, 8));        // done
CRGBSet chevron07(dialingLed(12, 14));      // done
CRGBSet chevron08(dialingLed(18, 20));      //
CRGBSet lockingChevron(dialingLed(25, 29)); // 25-27
CRGBSet chevron01(dialingLed(30, 32));      // done
CRGBSet chevron02(dialingLed(36, 38));      // done
CRGBSet chevron03(dialingLed(42, 44));      // done
CRGBSet chevron04(dialingLed(47, 49));      // 47. done
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
int wormholeArray[NUM_LEDS2][4] = {{0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}, {0, 255, 1, 0}};

void setup()
{
  initPlayer();

  Serial.begin(9600);

  button.begin();

  FastLED.addLeds<WS2812B, DATA_PIN, BGR>(dialingLed, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN2, BGR>(wormHoleLed, NUM_LEDS2);
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
  if (button.pressed())
  {
    buttonPressCount++;
    if (buttonPressCount > 3)
    {
      buttonPressCount = 0;
    } // increase to 3 if you want to play the theme without sound

    FastLED.clear();
    FastLED.show();
    // Stop playing MP3
    myDFPlayer.stop();
    startMillis = millis();
    wormholeActive = false;
    for (int i = 0; i <= NUM_LEDS2; i++)
    {
      wormholeArray[i][0] = 0;
      wormholeArray[i][1] = 255;
      wormholeArray[i][2] = 1;
      wormholeArray[i][3] = 0;
    }

    if (buttonPressCount == 1)
    {
      Serial.println(F("Button 1 Dialing"));
      myDFPlayer.stop();
      dialing = true;
      wormholeActive = false;
      myDFPlayer.volume(19);
      myDFPlayer.play(1);
    }
    else if (buttonPressCount == 2)
    {
      Serial.println(F("Button 2 Demo"));
      myDFPlayer.stop();
      dialing = false;
      wormholeActive = true;
      // myDFPlayer.volume(10);
      // myDFPlayer.loop(2);
      wormholeDemo();
    }
    else if (buttonPressCount == 3)
    {
      Serial.println(F("Button 3 Play theme"));
      myDFPlayer.stop();
      myDFPlayer.volume(19);
      myDFPlayer.play(1);
      dialing = false;
      wormholeActive = false;
    }
    else
    {
      Serial.println(F("No Button, stop all"));
      wormholeActive = false;
      dialing = false;
      myDFPlayer.stop();
      FastLED.clear();
    }
  }

  if (dialing)
  {
    dialingSequence();
  }

  if (wormholeActive)
  {
    wormholeSequence();
  }

  FastLED.show();
}

void wormholeDemo()
{
  // wormholeStartMillis = millis();
  if (wormholeActive)
  {
    chevron01 = CHSV(19, 255, 255); // is a fully saturated, fully bright color with hue ≈ 19 (a warm orange).
    chevron02 = CHSV(19, 255, 255);
    chevron03 = CHSV(19, 255, 255);
    chevron04 = CHSV(19, 255, 255);
    chevron05 = CHSV(19, 255, 255);
    chevron06 = CHSV(19, 255, 255);
    chevron07 = CHSV(19, 255, 255);
    chevron08 = CHSV(19, 255, 255);
    lockingChevron = CHSV(19, 255, 255);
  }
}

void wormholeSequence()
{
  long wormholeDuration = millis() - wormholeStartMillis;
  // Bright white, then puddle

  if (wormholeDuration < 1700)
  {
    wormholeBrightness++;
    if (wormholeBrightness > 210)
    {
      wormholeBrightness = 210;
    }
    for (int i = 0; i < NUM_LEDS2; i++)
    {
      wormHoleLed[i] = CRGB(wormholeBrightness, wormholeBrightness, wormholeBrightness);
    }
  }
  else
  {
    EVERY_N_MILLISECONDS(10)
    {
      wormholeBrightness = 0;
      int randomLED = random8(0, NUM_LEDS2);
      int randomColor = random8(0, 3);
      if (randomColor == 0)
      {
        randomColor = 100; // white?
      }
      else if (randomColor == 1)
      {
        randomColor = 130; // aqua
      }
      else
      {
        randomColor = 160; // blue
      }
      if (wormholeArray[randomLED][0] == 0)
      {
        wormholeArray[randomLED][0] = 1;
        wormholeArray[randomLED][3] = randomColor;
      }
    }

    for (int i = 0; i <= NUM_LEDS2; i++)
    {
      // If the LED is active, start or continue the fade in/out sequence.
      if (wormholeArray[i][0] == 1)
      {
        // Increment the brightness either up or down
        wormholeArray[i][1] = wormholeArray[i][1] + (1 * wormholeArray[i][2]);

        // Once full brightness reached, reverse direction.
        if (wormholeArray[i][1] >= 255)
        {
          wormholeArray[i][1] = 255;
          wormholeArray[i][2] = -wormholeArray[i][2];
        }

        // Once brightness fades back to zero, reset LED to inactive.
        if (wormholeArray[i][1] <= 0)
        {
          wormholeArray[i][0] = 0;
          wormholeArray[i][1] = 0;
          wormholeArray[i][2] = 1;
        }

        // Set each LED to blue, full saturation, brightness from loop.
        if (wormholeArray[i][3] == 100)
        {
          wormHoleLed[i] = CHSV(wormholeArray[i][3], 0, wormholeArray[i][1]);
        }
        else
        {
          wormHoleLed[i] = CHSV(wormholeArray[i][3], 255, wormholeArray[i][1]);
        }
      }
    }
  }
}

void dialingSequence()
{
  // How long dialing sequence has been running since button press
  long dialDuration = millis() - startMillis;

  if (dialDuration > 4998 && dialDuration < 5003)
  {
    lockedChevronFade = 0;
    chevronLocked = false;
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 11998 && dialDuration < 12003)
  {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 18998 && dialDuration < 19003)
  {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 25998 && dialDuration < 26003)
  {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 32998 && dialDuration < 33003)
  {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 39998 && dialDuration < 40003)
  {
    setLockingChevronLEDs(false);
  }
  if (dialDuration > 46998 && dialDuration < 47003)
  {
    setLockingChevronLEDs(true);
  }

  if (lockChevronOn)
  {
    lockingChevronLEDs();
  }

  if (dialDuration > 6198 && dialDuration < 7203)
  {
    lockedChevronLEDs(chevron01);
  }
  if (dialDuration > 10000 && dialDuration < 10005)
  {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 13198 && dialDuration < 14203)
  {
    lockedChevronLEDs(chevron02);
  }
  if (dialDuration > 20000 && dialDuration < 20005)
  {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 20198 && dialDuration < 21203)
  {
    lockedChevronLEDs(chevron03);
  }
  if (dialDuration > 27000 && dialDuration < 27005)
  {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 27198 && dialDuration < 28203)
  {
    lockedChevronLEDs(chevron06);
  }
  if (dialDuration > 33000 && dialDuration < 33005)
  {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 34198 && dialDuration < 35203)
  {
    lockedChevronLEDs(chevron07);
  }
  if (dialDuration > 40000 && dialDuration < 40005)
  {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 41198 && dialDuration < 42203)
  {
    lockedChevronLEDs(chevron08);
  }
  if (dialDuration > 44000 && dialDuration < 44005)
  {
    lockedChevronFade = 0;
    chevronLocked = false;
  }
  if (dialDuration > 48200 && dialDuration < 49205)
  {
    lockedChevronLEDs(chevron04);
    lockedChevronLEDs(chevron05);
  }

  if (dialDuration > 50400 && dialDuration < 50405)
  {
    wormholeActive = true;
    wormholeStartMillis = millis();
  }

  if (dialDuration > 66000)
  {
    wormholeActive = false;
    lockChevronOn = false;
    fadeToBlackBy(dialingLed, NUM_LEDS, 10);
    fadeToBlackBy(wormHoleLed, NUM_LEDS2, 10);
  }

  if (dialDuration > 67000)
  {
    dialing = false;
  }
}

// Reset variables for the next locking chevron run
void setLockingChevronLEDs(bool finalCall)
{
  lockChevronOn = true;
  lockChevronMillis = millis();
  lockChevronFade = 0;
  finalLock = finalCall;
}

// Sets the locking chevron LEDs on, slow fade up and down
void lockingChevronLEDs()
{
  // 2 seconds for top chevron to fade in, stay on, then fade out (.5, 1, .5 seconds)
  int lockingChevronDuration = millis() - lockChevronMillis;

  if (lockingChevronDuration < 500)
  {
    lockChevronFade = lockChevronFade + 2;
  }
  if (lockingChevronDuration > 1500 && !finalLock)
  {
    lockChevronFade = lockChevronFade - 2;
  }

  if (lockChevronFade > 255)
  {
    lockChevronFade = 255;
  }
  if (lockChevronFade < 0)
  {
    lockChevronFade = 0;
  }

  lockingChevron = CHSV(19, 255, lockChevronFade);

  if (lockingChevronDuration > 2000 && !finalLock)
  {
    lockChevronOn = false;
    lockingChevron = CHSV(19, 255, 0);
  }
}

// Sets locked chevron LEDs on after slow fade up
void lockedChevronLEDs(CRGBSet chevronNum)
{
  lockedChevronFade = lockedChevronFade + 2;

  if (lockedChevronFade > 255)
  {
    chevronNum = CHSV(19, 255, 255);
    chevronLocked = true;
  }

  if (!chevronLocked)
  {
    chevronNum = CHSV(19, 255, lockedChevronFade);
  }
}
