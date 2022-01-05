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
#include <Tone32.h>

#define PIN_TM1637_0_CLK 16
#define PIN_TM1637_0_DIO 4
#define PIN_TM1637_1_CLK 5
#define PIN_TM1637_1_DIO 17
#define PIN_BUZZER 12
#define PIN_BUTTON_CAPTURE 15
#define PIN_BUTTON_NEW_GAME 35
#define PIN_TOGGLE_DEC_HEX 13

// SX1509
const byte SX1509_I2C_ADDRESS = 0x3E;
SX1509 sx1509;
const int pins_sx1509_led[] = {0, 1, 2, 3, 4, 5, 6, 7};
const int pins_sx1509_toggle[] = {8, 9, 10, 11, 12, 13, 14, 15};
const byte ledMaxBrightnessPWMValue = 32;

TFT_eSPI tft = TFT_eSPI();

TM1637Display displayHex(PIN_TM1637_0_CLK, PIN_TM1637_0_DIO);
TM1637Display displayDec(PIN_TM1637_1_CLK, PIN_TM1637_1_DIO);

byte toggleValues;
bool showDecValues;
int turn;
const int numTurns = 10;
float score;

enum class Difficulty
{
  Easy,
  Medium,
  Hard
};

Difficulty difficulty;

static const char *difficultyText[6] = {"Easy", "Medium", "Hard"};

enum class State
{
  Home,
  Play,
  HighScores
};

State state = State::Home;

void NewGame()
{
  turn = 0;
  score = 0;
}

void Capture()
{
  if (++turn > numTurns)
  {
    // end game
  }
}

void ProcessScreen(bool forceUpdate = false)
{

  if (state == State::Home)
  {
  }
  else if (state == State::Play)
  {

    int target = 165;
    char buf[30];

    sprintf(buf, "Difficulty: %s", difficultyText[int(difficulty)]);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(buf, 15, 15);

    sprintf(buf, "Turn %u of %u", turn, numTurns);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(buf, 15, 40);

    sprintf(buf, "Score %f", score);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(buf, 15, 65);

    if (showDecValues)
    {
      sprintf(buf, "%3d", target);
    }
    else
    {
      sprintf(buf, "%3X", target);
    }

    tft.setFreeFont(&FreeSans24pt7b);
    tft.setTextSize(3);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString(buf, 160, 110);
  }
  else if (state == State::HighScores)
  {
  }
}

bool ProcessButtonsAndSwitch()
{

  bool activityFlag = false;

  if (digitalRead(PIN_BUTTON_NEW_GAME) == LOW)
  {
    NewGame();
    activityFlag = true;
  }

  if (digitalRead(PIN_BUTTON_CAPTURE) == LOW)
  {
    Capture();
    activityFlag = true;
  }

  static bool previousState;
  showDecValues = !digitalRead(PIN_TOGGLE_DEC_HEX);

  if (previousState != showDecValues)
  {
    previousState = showDecValues;
    activityFlag = true;
  }

  return activityFlag;
}

void ProcessTogglesAndLEDs()
{
  for (int n = 0; n < 8; n++)
  {
    bool toggleState = !sx1509.digitalRead(pins_sx1509_toggle[n]);
    bitWrite(toggleValues, n, toggleState);
    int brightness = toggleState == HIGH ? ledMaxBrightnessPWMValue : 0;
    sx1509.analogWrite(pins_sx1509_led[n], brightness);
  }
}

void ProcessSegmentDisplays()
{
  displayDec.showNumberDec(toggleValues, false);

  uint8_t digits[] = {0, 0, 0, 0};
  digits[2] = displayHex.encodeDigit((toggleValues & 0xF0) >> 4);
  digits[3] = displayHex.encodeDigit(toggleValues & 0x0F);
  displayHex.setSegments(digits);
}

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println("Binary Challenge");

  pinMode(PIN_BUTTON_CAPTURE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NEW_GAME, INPUT_PULLUP);
  pinMode(PIN_TOGGLE_DEC_HEX, INPUT_PULLUP);

  tft.init();
  delay(200);
  //tft.invertDisplay(false);
  delay(200);
  tft.setRotation(3);
  delay(200);

  tft.fillScreen(TFT_BLACK);

  /*
  tft.setCursor(10, 20);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(3);
  tft.println("Binary Challenge!");

  int w = tft.width();
  int h = tft.height();
  int t = 3;
  tft.fillRect(0, 0, w, t, TFT_CYAN);
  tft.fillRect(w - t, 0, t, h, TFT_CYAN);
  tft.fillRect(0, h - 3, w, t, TFT_CYAN);
  tft.fillRect(0, 0, 0 + t, h, TFT_CYAN);

  tft.fillRect(0, 50, tft.width(), t, TFT_CYAN);
  tft.fillRect(0, 205, tft.width(), t, TFT_CYAN);
*/

  if (sx1509.begin(SX1509_I2C_ADDRESS) == false)
  {
    Serial.println("Failed to communicate. Check wiring and address of SX1509.");
  }

  for (int i = 0; i < 8; i++)
  {
    sx1509.pinMode(pins_sx1509_led[i], ANALOG_OUTPUT);
    sx1509.analogWrite(pins_sx1509_led[i], 0);
    sx1509.pinMode(pins_sx1509_toggle[i], INPUT);
    sx1509.pinMode(pins_sx1509_toggle[i], INPUT_PULLUP);
  }

  displayDec.setBrightness(0x03);
  displayHex.setBrightness(0x03);
}

void loop()
{

  //tone(PIN_BUZZER, NOTE_C4, 500, 0);
  //noTone(PIN_BUZZER, 0);
  //delay(2000);

  state = State::Play;

  bool activityFlag = ProcessButtonsAndSwitch();

  ProcessTogglesAndLEDs();

  ProcessSegmentDisplays();

  if (activityFlag)
    ProcessScreen();
}