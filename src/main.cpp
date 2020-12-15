#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 40
#define BRIGHTNESS 255
#define COLOR_ORDER GRB
#define CHIPSET WS2811

#define INTERRUPT_PIN 2
#define RESETTED_DELAY 50

#define WALKIN_TIME 7 //Seconds
#define WALKIN_FADEIN_STEP 15

CRGB leds[NUM_LEDS];

int delayval = 50;
int resetDelay = RESETTED_DELAY;
bool lightIsOn = false;
volatile bool movementFound = false;

void movementDetected()
{
  movementFound = true;
}

void setup()
{
  //PIR Sensor
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), movementDetected, CHANGE);

  //FastLED Setup
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void LightsWalkIn()
{
  int mDelay = WALKIN_TIME * 1000 / NUM_LEDS / (255 / WALKIN_FADEIN_STEP);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    for (int zoom = WALKIN_FADEIN_STEP; zoom <= 255; zoom += WALKIN_FADEIN_STEP)
    {
      leds[i].addToRGB(WALKIN_FADEIN_STEP);
      FastLED.show();
      delay(mDelay);
    }
  }
}

void LightsOn()
{
  LightsWalkIn();
  lightIsOn = true;
  return;

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::White;
    FastLED.show();
    delay(delayval);
    lightIsOn = true;
  }
}

void LightsOff()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(delayval);
    lightIsOn = false;
  }
}

void loop()
{
  //LICHT AUS + BEWEGUNG

  //BEWEGUNG
  if (movementFound == true)
  {
    movementFound = false;
    resetDelay = RESETTED_DELAY;

    if (lightIsOn == false)
    {
      LightsOn();
    }
  }

  // LICHT IST AN
  if (lightIsOn)
  {

    //Ausschalten wenn delay abgeschlossen
    if (resetDelay <= 0)
    {
      LightsOff();
    }
    else
    {
      delay(delayval);
      resetDelay--;
    }
  }
}