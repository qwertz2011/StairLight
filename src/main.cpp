#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 40
#define INTERRUPT_PIN 2
#define RESETTED_DELAY 50

int delayval = 150;
int resetDelay = RESETTED_DELAY;
bool lightIsOn = false;
volatile bool movementFound = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, 6, NEO_GRB + NEO_KHZ800);

void movementDetected()
{
  movementFound = true;
}

void setup()
{
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), movementDetected, CHANGE);

  pixels.begin();
  pixels.setBrightness(255);
}

void LightsWalkIn()
{
  int step = 51;
  int stepCount = 5;

  for (int i = 0; i < NUMPIXELS; i++)
  {
    for (int x = 0; x < stepCount && i - x >= 0; x++)
    {
      int stepVal = x * step + step;
      int nextStepVal = x * (step + 1) + step;
      for (int zoom = stepVal; zoom <= nextStepVal; zoom++)
      {
        pixels.setPixelColor(i - x, stepVal, stepVal, stepVal);
        pixels.show();
      }
    }

    pixels.show();
    delay(delayval);
  }
}

void LightsOn()
{
  LightsWalkIn();
  lightIsOn = true;
  return;

  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, 255, 255, 255);
    pixels.show();
    delay(delayval);
    lightIsOn = true;
  }
}

void LightsOff()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, 0, 0, 0);
    pixels.show();
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