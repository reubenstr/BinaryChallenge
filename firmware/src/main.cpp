/*
  PROJECT:
    Binary Challege

  DESCRIPTION:
    Binary to DEC/HEX conversion game competing for fasteset user input. 

  DATE:
    January 2021

  AUTHOR:
    Reuben Strangelove
    www.metaphasiclabs.com

  MCU:

  PERIPHERALS:
    SX1509 port expander - 8 LEDs and 8 toggle switches

  TODO:


*/

#include <Arduino.h>
#include <TM1637Display.h> 
#include <Wire.h>
#include <SparkFunSX1509.h>
#include "SPI.h"
#include <TFT_eSPI.h>


#define PIN_TM1637_0_CLK 2
#define PIN_TM1637_0_DIO 3
#define PIN_TM1637_1_CLK 4
#define PIN_TM1637_1_DIO 5
#define PIN_BUZZER 6
//#define PIN_BUTTON_CAPTURE 7
/*
#define PIN_LED_BUILTIN 13
#define PIN_BUTTON_NEW_GAME A0
*/

// SX1509
SX1509 sx1509;
const byte SX1509_I2C_ADDRESS = 0x3E;
const int pins_sx1509_led[] = {0, 1, 2, 3, 4, 5, 6, 7};
const int pins_sx1509_toggle[] = {8, 9, 10, 11, 12, 13, 14, 15};


TFT_eSPI tft = TFT_eSPI();

//TM1637Display display0(PIN_TM1637_0_CLK, PIN_TM1637_0_DIO);
//TM1637Display display1(PIN_TM1637_1_CLK, PIN_TM1637_1_DIO);

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println("Binary Challenge");

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 20);
  tft.setTextColor(TFT_BLUE);
  tft.setTextSize(2);
  tft.println("Binary Challenge!");

  Wire.begin();

  if (sx1509.begin(SX1509_I2C_ADDRESS) == false)
  {
    Serial.println("Failed to communicate. Check wiring and address of SX1509.");
  }

  for (int i = 0; i < 8; i++)
  {
    sx1509.pinMode(pins_sx1509_led[i], ANALOG_OUTPUT);
    sx1509.pinMode(pins_sx1509_toggle[i], INPUT);
    sx1509.pinMode(pins_sx1509_toggle[i], INPUT_PULLUP);
  }
}

void loop()
{

  for (int i = 0; i < 8; i++)
  {
    int brightness = sx1509.digitalRead(pins_sx1509_toggle[i]) == LOW ? 127 : 0;
    sx1509.analogWrite(pins_sx1509_led[i], brightness);
  }
}