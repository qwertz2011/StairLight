#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define LED_PIN 6
#define INTERRUPT_PIN_FIRST_FLOOR 3
#define INTERRUPT_PIN_GROUND_FLOOR 2

#define NUM_LEDS 300
#define BRIGHTNESS 10
#define COLOR_ORDER GRB
#define CHIPSET WS2811

#define LIGHTSON_EFFECT_DURATION 3000
#define LIGHTSOFF_EFFECT_DURATION 3000
#define NO_MOVEMENT_LIGHTSOFF_DELAY 20000

#define WALKIN_FADEIN_STEP 15

enum Direction
{
  None = 0,
  Up = 1,
  Down = 2
};

// CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;
bool lightIsOn = false;
volatile bool movementFound = false;
volatile Direction direction = Direction::None;
unsigned long timerLightsOff = 0;

void movementDetectedFirstFloor()
{
  movementFound = true;
  direction = Direction::Down;
}

void movementDetectedGroundFloor()
{
  movementFound = true;
  direction = Direction::Up;
}

void setup()
{
  randomSeed(analogRead(0));

  //PIR Sensor
  pinMode(INTERRUPT_PIN_FIRST_FLOOR, INPUT);
  pinMode(INTERRUPT_PIN_GROUND_FLOOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_FIRST_FLOOR), movementDetectedFirstFloor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_GROUND_FLOOR), movementDetectedGroundFloor, CHANGE);

  //FastLED Setup
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);

  Serial.begin(115200);
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

  if (direction == Direction::Down)
  {
    for (int z = NUM_LEDS - 1; z > 0; z -= sectionSize)
    {
      uint8_t first = z;
      uint8_t last = max(0, z - sectionSize + 1);

      CRGBSet currentSet = leds(first, last);
      currentSet.fill_gradient_RGB(CRGB::White, CRGB::Blue);
      FastLED.show();
      delay(mDelay); //show
    }
  }
  else
  {
    for (int z = 0; z < NUM_LEDS; z += sectionSize)
    {
      CRGBSet currentSet = leds(z, z + sectionSize - 1);
      currentSet.fill_gradient_RGB(CRGB::White, CRGB::Blue);
    }
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
  int mDelay = SingleLedDelay();
  if (direction == Direction::Down)
  {
    for (int ZZZ = 0; ZZZ < NUM_LEDS - 1; ZZZ++)
    {
      leds[ZZZ] = CRGB::FairyLight;
      FastLED.delay(mDelay);
    }
  }
  else
  {
    for (int ZZZ = NUM_LEDS - 1; ZZZ >= 0; ZZZ--)
    {
      leds[ZZZ] = CRGB::White;
      FastLED.delay(mDelay);
    }
  }
}

void LightsOnRandomColor()
{
  int mDelay = SingleLedDelay();
  for (int i = 0; i < NUM_LEDS; i++)
  {
    uint8_t r1 = random();
    uint8_t r2 = random();
    uint8_t r3 = random();

    leds[i] = CRGB(r1, r2, r3);
    FastLED.delay(mDelay);
  }
}

void AllLightsOnRandomColor()
{
  for (int z = 0; z <= 10; z++)
  {
    uint8_t r1 = random();
    uint8_t r2 = random();
    uint8_t r3 = random();

    leds.fill_solid(CRGB(r1, r2, r3));
    delay(250);
  }
}

void LightsOnDefault()
{
  int mDelay = SingleLedDelay();

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::White;
    FastLED.show();
    delay(mDelay);
  }
}

void LightsOn()
{
  int randomLight = (int)random(4);
  randomLight = 1;

  Serial.println("Effect " + String(randomLight));

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

  for (int buh = 1; buh < NUM_LEDS - 1; buh++)
  {
    leds[buh] = CRGB::Black;
    FastLED.show();
    delay(mDelay);
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

  // CRGBSet SetA = leds(0, 120);
  // CRGBSet SetB = leds(121, 299);

  // while (true)
  // {
  //   SetB = CRGB::DarkBlue;
  //   FastLED.delay(1000);

  //   SetB = CRGB::DarkRed;
  //   FastLED.delay(1000);

  //   SetB = CRGB::DarkGreen;
  //   FastLED.delay(1000);

  //   FastLED.showColor(CRGB::Yellow);
  //   delay(500);
  // }

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
      FastLED.clear(true);
    }

    //TODO - MAYBE IDLE ANIMATION
    delay(100);
  }
}
#pragma endregion