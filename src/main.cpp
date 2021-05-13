#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#define FASTLED_INTERNAL //Disable Debug Version Number of FastLED
#include <FastLED.h>

#define AUX_POWER_PIN 10
#define LED_PIN 7
#define INTERRUPT_PIN_FIRST_FLOOR (uint8_t)3
#define INTERRUPT_PIN_GROUND_FLOOR (uint8_t)2
#define AMBIENT1_PIN (uint8_t) A2
#define AMBIENT2_PIN (uint8_t) A3

#define NUM_LEDS 300
#define BRIGHTNESS 10
#define COLOR_ORDER GRB
#define CHIPSET WS2812

#define MAIN_COLOR CRGB::FairyLight
#define ACCENT_COLOR CRGB::Blue

#define AMBIENT_DAYLIGHT_THRESHOLD 200ul

#define READ_AMBIENT_INTERVAL 500ul
#define NO_MOVEMENT_LIGHTSOFF_DELAY 20000ul
#define NO_AUX_POWER_REQUIRED_DELAY 120000ul

#define LIGHTSON_EFFECT_DURATION 3000ul
#define LIGHTSOFF_EFFECT_DURATION 3000ul

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
bool auxPowerOn = false;
bool daylight = false;

volatile bool movementFound = false;
volatile Direction direction = Direction::None;

void movementDetectedFirstFloor()
{
  if (movementFound)
  {
    return;
  }
  direction = Direction::Down;
  movementFound = true;
}

void movementDetectedGroundFloor()
{
  if (movementFound)
  {
    return;
  }
  direction = Direction::Up;
  movementFound = true;
}

int SingleLedDelay(double factor = 1)
{
  return (int)LIGHTSON_EFFECT_DURATION / (NUM_LEDS * factor);
}
/*
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
      currentSet.fill_gradient_RGB(MAIN_COLOR, ACCENT_COLOR);
      FastLED.show();
      delay(mDelay); //show
    }
  }
  else
  {
    for (int z = 0; z < NUM_LEDS; z += sectionSize)
    {
      CRGBSet currentSet = leds(z, z + sectionSize - 1);
      currentSet.fill_gradient_RGB(MAIN_COLOR, ACCENT_COLOR);
    }
  }
}*/

void LightsOnFadeAll()
{
  unsigned long mDelay = LIGHTSON_EFFECT_DURATION / NUM_LEDS / (255 / 10);

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
      leds[ZZZ] = MAIN_COLOR;
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
    leds[i] = MAIN_COLOR;
    FastLED.show();
    delay(mDelay);
  }
}

void TurnAuxPowerOn()
{
  digitalWrite(AUX_POWER_PIN, HIGH);
  auxPowerOn = true;
}

void TurnAuxPowerOff()
{
  digitalWrite(AUX_POWER_PIN, LOW);
  auxPowerOn = false;
}

void LightsOn()
{
  int randomLight = (int)random(4);
  randomLight = 1;

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

  // // case 4:
  // //   LightsOnSections();
  // //  break;
  case 5:
  default:
    LightsOnFadeAll();
    break;
  }

  lightIsOn = true;
}

void LightsOffDefault()
{
  int mDelay = SingleLedDelay();

  for (int led = 0; led < NUM_LEDS; led++)
  {
    leds[led] = CRGB::Black;
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
  int randomLight = (int)random(0,1);

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

uint16_t GetAmbient(uint8_t pin)
{
  analogRead(pin); //Test: Read two times zu get better readings
  return analogRead(pin);
}

uint16_t ambient1 = 0;
uint16_t ambient2 = 0;
uint16_t ambientAverage = 0;

void ReadAmbient()
{
  ambient1 = GetAmbient(AMBIENT1_PIN);
  ambient2 = GetAmbient(AMBIENT2_PIN);
  ambientAverage = (ambient1 + ambient2) / 2;
  daylight = ambientAverage >= AMBIENT_DAYLIGHT_THRESHOLD;
}

#include <TaskTimer.h>
TaskTimer readAmbientTimer = TaskTimer(ReadAmbient, READ_AMBIENT_INTERVAL, false);
TaskTimer lightsOffTimer = TaskTimer(LightsOff, NO_MOVEMENT_LIGHTSOFF_DELAY, true);
TaskTimer auxPowerOffTimer = TaskTimer(TurnAuxPowerOff, NO_AUX_POWER_REQUIRED_DELAY, true);

void setup()
{
  randomSeed(analogRead(0));

  pinMode(13, OUTPUT);

  //AUX Power - Default Power Off
  pinMode(AUX_POWER_PIN, OUTPUT);
  digitalWrite(AUX_POWER_PIN, LOW);

  //PIR Sensor
  pinMode(INTERRUPT_PIN_FIRST_FLOOR, INPUT);
  pinMode(INTERRUPT_PIN_GROUND_FLOOR, INPUT);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_FIRST_FLOOR), movementDetectedFirstFloor, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_GROUND_FLOOR), movementDetectedGroundFloor, RISING);

  //FastLED Setup
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);
}

void loop()
{
  //BEWEGUNG
  if (movementFound)
  {
    delay(1);
    if (direction == Direction::Down)
    {
      if (digitalRead(INTERRUPT_PIN_FIRST_FLOOR) != HIGH)
      {
        return;
      }
    }
    else if (direction == Direction::Up)
    {
      if (digitalRead(INTERRUPT_PIN_GROUND_FLOOR) != HIGH)
      {
        return;
      }
    }
    else
    {
      return;
    }

    //no Daylight
    if (!daylight)
    {
      //Power ON
      if (!auxPowerOn)
      {
        TurnAuxPowerOn();
        delay(50);

        auxPowerOffTimer.Activate(true);
      }

      //Lights ON
      if (!lightIsOn)
      {
        LightsOn();
        lightsOffTimer.Activate(true);
      }
    }

    //Reset Movement Trigger
    movementFound = false;

    lightsOffTimer.ResetTimer();
  }

  if (lightIsOn)
  {
    auxPowerOffTimer.ResetTimer();
    readAmbientTimer.Deactivate();
  }
  else
  {
    readAmbientTimer.Activate();
  }

  readAmbientTimer.Tick();
  lightsOffTimer.Tick();
  auxPowerOffTimer.Tick();

  //TODO - MAYBE IDLE ANIMATION
  delay(100);
}
