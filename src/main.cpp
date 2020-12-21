#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 300
#define BRIGHTNESS 255
#define COLOR_ORDER GRB
#define CHIPSET WS2811

#define INTERRUPT_PIN 2
#define RESETTED_DELAY 50

#define WALKIN_TIME 7 //Seconds
#define WALKIN_FADEIN_STEP 15

#define FADEIN_TIME 5 //Seconds

CRGB leds[NUM_LEDS];

int delayval = 100;
int resetDelay = RESETTED_DELAY;
bool lightIsOn = false;
volatile bool movementFound = false;

void movementDetected()
{
  movementFound = true;
}

void setup()
{
  randomSeed(analogRead(0));

  //PIR Sensor
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), movementDetected, CHANGE);

  //FastLED Setup
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void LightsOnFadeAll()
{

  int mDelay = FADEIN_TIME * 1000 / NUM_LEDS / (255 / 10);

  for (int z = 0; z <= 255; z += 10)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(z, z, z);
      FastLED.show();
    }
    delay(mDelay);
  }
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

void LightsOnRandomColor()
{
  while (true)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint8_t r1 = random();
      uint8_t r2 = random();
      uint8_t r3 = random();

      leds[i] = CRGB(r1, r2, r3);
      FastLED.show();
      // delay(delayval);
    }
  }
}

void AllLightsOnRandomColor()
{
  while (true)
  {
    uint8_t r1 = random();
    uint8_t r2 = random();
    uint8_t r3 = random();

    FastLED.showColor(CRGB(r1, r2, r3));
    delay(250);
  }
}

void LightsOnDefault()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::White;
    FastLED.show();
    // delay(delayval);
  }
}

void LightsOn()
{
  int randomLight = (int)random(4);

  switch (randomLight)
  {
  case 0:
    LightsOnDefault();
    break;

  case 1:
    LightsWalkIn();
    break;

  case 2:
    LightsOnRandomColor();
    break;

  case 3:
    AllLightsOnRandomColor();
    break;

  case 4:
  default:
    LightsOnFadeAll();
    break;
  }

  lightIsOn = true;
}

void LightsOff()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(delayval);
  }

  lightIsOn = false;
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

  return;

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