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



  TODO:



*/



#include <Arduino.h>
#include <TM1637Display.h> // https://github.com/avishorp/TM1637
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_STMPE610.h>
#include <Wire.h>
#include <SparkFunSX1509.h>

// Arduino Nano Pins
#define PIN_TM1637_0_CLK 2
#define PIN_TM1637_0_DIO 3
#define PIN_TM1637_1_CLK 4
#define PIN_TM1637_1_DIO 5
#define PIN_BUZZER 6
#define PIN_BUTTON_CAPTURE 7
#define STMPE_CS 8
#define TFT_DC 9
#define TFT_CS 10
#define PIN_BUTTON_NEW_GAME A0

// SX1509 Pins
#define PIN_SX1509_LED_0 8
#define PIN_SX1509_LED_1 9
#define PIN_SX1509_LED_2 10
#define PIN_SX1509_LED_3 11
#define PIN_SX1509_LED_4 12
#define PIN_SX1509_LED_5 13
#define PIN_SX1509_LED_6 14
#define PIN_SX1509_LED_7 15
#define PIN_SX1509_TOGGLE_0 0
#define PIN_SX1509_TOGGLE_1 1
#define PIN_SX1509_TOGGLE_2 2
#define PIN_SX1509_TOGGLE_3 3
#define PIN_SX1509_TOGGLE_4 4
#define PIN_SX1509_TOGGLE_5 5
#define PIN_SX1509_TOGGLE_6 6
#define PIN_SX1509_TOGGLE_7 7


const byte SX1509_I2C_ADDRESS = 0x3E;  // SX1509 I2C address
SX1509 sx1509; 

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

TM1637Display display0(PIN_TM1637_0_CLK, PIN_TM1637_0_DIO);
TM1637Display display1(PIN_TM1637_1_CLK, PIN_TM1637_1_DIO);





void setup() 
{
  if (!sx1509.begin(SX1509_I2C_ADDRESS))
  {
    Serial.println("Failed to communicate.");
    while (1) ;
  }
}

void loop() 
{
  
  
}