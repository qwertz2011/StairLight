#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#define FASTLED_INTERNAL //Disable Debug Version Number of FastLED
#include <FastLED.h>

#define AUX_POWER_PIN 10
#define LED_PIN 7
#define INTERRUPT_PIN_FIRST_FLOOR 3
#define INTERRUPT_PIN_GROUND_FLOOR 2
#define AMBIENT1_PIN (uint8_t) A2
#define AMBIENT2_PIN (uint8_t) A3

#define NUM_LEDS 300
#define BRIGHTNESS 10
#define COLOR_ORDER GRB
#define CHIPSET WS2812

#define MAIN_COLOR CRGB::FairyLight
#define ACCENT_COLOR CRGB::Blue

#define READ_AMBIENT_INTERVAL 500ul
#define AMBIENT_DAYLIGHT_THRESHOLD 200u
#define LIGHTSON_EFFECT_DURATION 3000u
#define LIGHTSOFF_EFFECT_DURATION 3000u
#define NO_MOVEMENT_LIGHTSOFF_DELAY 20000u
#define NO_AUX_POWER_REQUIRED_DELAY 120000u

#define WALKIN_FADEIN_STEP 15

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

unsigned long timerLightsOff = 0;
unsigned long timerAuxPowerOff = 0;
unsigned long timerReadAmbient = 0;

volatile bool movementFound = false;
volatile Direction direction = Direction::None;

void PrintText(String text, bool clear = false)
{
  if (clear)
  {
    display.clearDisplay();
  }
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(text); //Textzeile ausgeben
  display.display();
}

void movementDetectedFirstFloor()
{
  direction = Direction::Down;
  movementFound = true;
}

void movementDetectedGroundFloor()
{
  direction = Direction::Up;
  movementFound = true;
}

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

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
  }

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_FIRST_FLOOR), movementDetectedFirstFloor, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_GROUND_FLOOR), movementDetectedGroundFloor, RISING);

  //FastLED Setup
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);
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

void loop()
{

  //Ambient Light Level
  if (timerReadAmbient <= millis())
  {
    ambient1 = GetAmbient(AMBIENT1_PIN);
    ambient2 = GetAmbient(AMBIENT2_PIN);
    ambientAverage = (ambient1 + ambient2) / 2;

    daylight = ambientAverage <= AMBIENT_DAYLIGHT_THRESHOLD;

    timerReadAmbient = millis() + READ_AMBIENT_INTERVAL;
  }

  //LICHT AUS

  //BEWEGUNG
  if (movementFound)
  {

    if (direction == Direction::Down)
    {
      PrintText("OG", true);
    }
    else if (direction == Direction::Up)
    {
      PrintText("EG", true);
    }
    else
    {
      PrintText("NO DIR", true);
    }

    delay(1000);

    movementFound = false;

    if (!daylight)
    {
      if (!auxPowerOn)
      {
        TurnAuxPowerOn();
        delay(50);

        timerAuxPowerOff = millis() + NO_AUX_POWER_REQUIRED_DELAY;
      }

      if (!lightIsOn)
      {
        LightsOn();
      }
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
  }

  //Licht aus AUX Power AN - TurnOff Check
  if (!lightIsOn && auxPowerOn)
  {
    //AUX Power Off
    if (timerAuxPowerOff <= millis())
    {
      TurnAuxPowerOff();
    }
  }

  //TODO - MAYBE IDLE ANIMATION
  delay(100);

  PrintText("Ambient: " + String(ambient1) + "|" + String(ambient2) + "|" + String(ambientAverage) + "\nDaylight: " + String(daylight), true);
}
