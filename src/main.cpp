#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define LED_PIN 6
#define INTERRUPT_PIN 2

#define NUM_LEDS 150
#define BRIGHTNESS 10
#define COLOR_ORDER GRB
#define CHIPSET WS2811

#define LIGHTSON_EFFECT_DURATION 3000
#define LIGHTSOFF_EFFECT_DURATION 3000
#define NO_MOVEMENT_LIGHTSOFF_DELAY 5000

#define WALKIN_FADEIN_STEP 15

// CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;
bool lightIsOn = false;
volatile bool movementFound = false;
unsigned long timerLightsOff = 0;

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

#pragma region LightsOn

int SingleLedDelay(double factor = 1)
{
  return (int)LIGHTSON_EFFECT_DURATION / (NUM_LEDS * factor);
}

void LightsOnSections()
{
  int mDelay = SingleLedDelay(0.067);
  uint8_t sectionSize = NUM_LEDS / 20;

  for (int z = 0; z < NUM_LEDS; z += sectionSize)
  {
    CRGBSet currentSet = leds(z, z + sectionSize);
    currentSet = CRGB::White;
    FastLED.delay(mDelay); //show
  }
}

void LightsOnFadeAll()
{

  int mDelay = LIGHTSON_EFFECT_DURATION / NUM_LEDS / (255 / 10);

  for (int z = 0; z <= 255; z += 20)
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
  int mDelay = LIGHTSON_EFFECT_DURATION / NUM_LEDS / (255 / WALKIN_FADEIN_STEP);

  FastLED.showColor(CRGB::White);
  return;

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
    FastLED.delay(250);
  }
}

void LightsOnDefault()
{
  int mDelay = SingleLedDelay();

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::White;
    FastLED.show();
    FastLED.delay(mDelay);
  }
}

void LightsOn()
{
  int randomLight = (int)random(4);
  randomLight = 4;

  switch (randomLight)
  {
  case 0:
    // LightsOnDefault();
    LightsWalkIn();
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
    LightsOnSections();
    break;
  case 5:
  default:
    LightsOnFadeAll();
    break;
  }

  lightIsOn = true;
}
#pragma endregion

#pragma region LightsOff

void LightsOffDefault()
{
  int mDelay = SingleLedDelay();
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
    FastLED.show();
    FastLED.delay(mDelay);
  }
}

void LightsOffInstant()
{
  FastLED.clear(true);
}

void LightsOff()
{

  int randomLight = (int)random(4);
  randomLight = 0;

  switch (randomLight)
  {
  case 0:
    LightsOffDefault();
    break;
  case 1:
  default:
    LightsOffInstant();
    break;
  }

  lightIsOn = false;
}

#pragma endregion

#pragma region Main

void loop()
{
  //LICHT AUS + BEWEGUNG

  //BEWEGUNG
  if (movementFound)
  {
    movementFound = false;

    if (!lightIsOn)
    {
      LightsOn();
    }

    //Time is up - Lights off
    timerLightsOff = millis() + NO_MOVEMENT_LIGHTSOFF_DELAY;
  }

  // LICHT IST AN
  if (lightIsOn)
  {

    //Ausschalten wenn delay abgeschlossen
    if (timerLightsOff <= millis())
    {
      LightsOff();
    }

    //TODO - MAYBE IDLE ANIMATION
  }
}
#pragma endregion