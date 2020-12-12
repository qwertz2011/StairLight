#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 20
#define INTERRUPT_PIN 2
#define RESETTED_DELAY 50

int delayval = 200;
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
  Serial.begin(9600);
  pixels.setBrightness(255);
}

void LightsOn()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    SetMyPixel(i, 255, 255, 255);
    pixels.show();
    delay(delayval);
    lightIsOn = true;
  }
}

void LightsOff()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    SetMyPixel(i, 0, 0, 0);
    pixels.show();
    delay(delayval);
    lightIsOn = false;
  }
}

void loop()
{

  // for (int i = 0; i < NUMPIXELS; i++)
  // {
  //   SetMyPixel(i, 255, 255, 255);
  //   SetMyPixel(i - 3, 0, 0, 0);
  //   pixels.show();
  //   delay(delayval);
  // }

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
      Serial.println("Reset" + resetDelay);
      delay(delayval);
      resetDelay--;
    }
  }
}