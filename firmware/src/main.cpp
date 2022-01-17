/*
    Binary Challege
    Reuben Strangelove
    www.metaphasiclabs.com

    January 2021
    1.0

  DESCRIPTION:
    Binary to DEC/HEX conversion game competing for fastest user input. 
   
  MCU:
    ESP32 (DOIT ESP32 DEVKIT V1)

  PERIPHERALS:
	  TFT LCD 2.8" w/touch - ILIXXXX (HiLetGo)
    SX1509 port expander - 8 LEDs and 8 toggle switches

	NOTES:
		Source code is hardware agnostic and should support other MCU's in the Arduino
		ecosystem such as a STM32 pending the plaform is 3.3v per TFT LCD requirement.

  KNOWN ISSUES:
    Very rarely communication between the MCU and SX1509 fails and wrong GPIO states are transmitted.

  TODO:
	  High score leaderboard - allow user to input initials via touch screen.

*/
#include <Arduino.h>
#include <TM1637Display.h>
#include <Wire.h>
#include <SparkFunSX1509.h>
#include "SPI.h"
#include <TFT_eSPI.h>
#include <Tone32.h>
#include <main.h>
#include <utilities.h>
#include <splash.h>
#define PIN_TM1637_0_CLK 16
#define PIN_TM1637_0_DIO 4
#define PIN_TM1637_1_CLK 5
#define PIN_TM1637_1_DIO 17
#define PIN_BUZZER 12
#define PIN_BUTTON_CAPTURE 15
#define PIN_BUTTON_NEW_GAME 27
#define PIN_TOGGLE_DEC_HEX 13

/*
TFT LCD pins defined in TFT_eSPI library's User_Setup.h
#define ILI9341_DRIVER 
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
#define TFT_CS 32
#define TFT_DC 25
#define TFT_SCLK 18
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_RST 33
#define TOUCH_CS 26    
*/

const byte SX1509_I2C_ADDRESS = 0x3E;
SX1509 sx1509;
const int pins_sx1509_led[] = {0, 1, 2, 3, 4, 5, 6, 7};
const int pins_sx1509_toggle[] = {8, 9, 10, 11, 12, 13, 14, 15};
const byte ledMaxBrightnessPWMValue = 32;

TFT_eSPI tft = TFT_eSPI();

TM1637Display displayHex(PIN_TM1637_0_CLK, PIN_TM1637_0_DIO);
TM1637Display displayDec(PIN_TM1637_1_CLK, PIN_TM1637_1_DIO);

Game game;

void PlayTone(Tone t)
{
  if (t == Tone::Startup)
  {
    tone(PIN_BUZZER, NOTE_B6, 125, 0);
    tone(PIN_BUZZER, NOTE_E6, 500, 0);
  }
  else if (t == Tone::NewGame)
  {
    tone(PIN_BUZZER, NOTE_C5, 125, 0);
  }
  else if (t == Tone::CaptureSuccess)
  {
    tone(PIN_BUZZER, NOTE_C5, 125, 0);
    tone(PIN_BUZZER, NOTE_C6, 250, 0);
  }
  else if (t == Tone::CaptureFail)
  {
    tone(PIN_BUZZER, NOTE_C4, 50, 0);
  }
  else if (t == Tone::TogglesReset)
  {
    tone(PIN_BUZZER, NOTE_C5, 125, 0);
    tone(PIN_BUZZER, NOTE_CS5, 125, 0);
    tone(PIN_BUZZER, NOTE_D5, 125, 0);
  }
  else if (t == Tone::OutofTime)
  {
    tone(PIN_BUZZER, NOTE_C5, 125, 0);
    tone(PIN_BUZZER, NOTE_C4, 1000, 0);
  }
  else if (t == Tone::EndOfGame)
  {
    int steps = 10;
    for (int i = 0; i < steps; i++)
    {
      tone(PIN_BUZZER, (NOTE_C7 - NOTE_C6) / steps * i, 50, 0);
    }
    tone(PIN_BUZZER, (NOTE_C7 - NOTE_C6) / steps * steps, 500, 0);
  }

  noTone(PIN_BUZZER, 0);
}

void UpdateDisplay(Display display)
{
  char buf[30];
  const int yTimer = 90;
  if (display == Display::Splash)
  {
    tft.pushImage(0, 0, splashWidth, splashHeight, splash);
  }
  else if (display == Display::SelectDifficulty)
  {
    tft.fillScreen(TFT_BLACK);

    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("Change difficulty using toggles:", 160, 20);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("1 = easy", 90, 50);
    tft.drawString("2 = meduim", 90, 75);
    tft.drawString("4 = hard", 90, 100);

    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("(press capture when ready)", 160, 200);
  }
  else if (display == Display::UpdateDifficulty)
  {
    tft.fillRect(80, 135, 160, 50, difficultyColors[int(game.difficulty)]);
    tft.setFreeFont(&FreeSans18pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_BLACK, difficultyColors[int(game.difficulty)]);
    tft.drawString(difficultyTextUC[int(game.difficulty)], 160, 145);
  }
  else if (display == Display::ResetTogglesOnly)
  {
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("(set toggles to zero)", 160, 120);
  }
  else if (display == Display::Target)
  {
    tft.fillRect(15, 101, tft.width() - 15, tft.height() - 101, TFT_BLACK);

    if (game.showDecValues)
    {
      sprintf(buf, "%d", game.target);
    }
    else
    {
      sprintf(buf, "%02X", game.target);
    }

    tft.setFreeFont(&FreeSans24pt7b);
    tft.setTextSize(3);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(buf, 160, 115);
  }
  else if (display == Display::Countdown)
  {
    static unsigned long start;
    if (millis() - start > 35)
    {
      start = millis();
      int pixelPadding = 15;
      int timeLeft = millis() - game.startTimeToCapture;
      int halfWidth = map(timeLeft, 0, timeMSAllowedToCapture, 0, tft.width() / 2 - pixelPadding + 5);
      tft.fillRect(pixelPadding, yTimer, halfWidth, 10, TFT_RED);

      tft.fillRect(halfWidth + pixelPadding, yTimer, tft.width() - pixelPadding - halfWidth * 2 - pixelPadding, 10, TFT_CYAN);

      tft.fillRect(tft.width() - pixelPadding - halfWidth, yTimer, halfWidth, 10, TFT_RED);
    }
  }
  else if (display == Display::GameInfo)
  {
    tft.fillScreen(TFT_BLACK);

    sprintf(buf, "Difficulty: %s     ", difficultyText[int(game.difficulty)]);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(buf, 15, 15);

    sprintf(buf, "Turn: %u of %u     ", game.turn, game.numTurns);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(buf, 15, 40);

    sprintf(buf, "Score: %u     ", game.scoreTotal);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(buf, 15, 65);
  }
  else if (display == Display::ResetTogglesWithScore)
  {
    sprintf(buf, "score = %u", game.score);
    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString(buf, 160, 120);

    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("(set toggles to zero)", 160, 180);
  }
  else if (display == Display::FinalScore)
  {
    tft.fillScreen(TFT_BLACK);

    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    sprintf(buf, "%u", game.scoreTotal);
    tft.drawString("FINAL", 160, 35);
    tft.drawString("SCORE", 160, 100);
    tft.drawString(buf, 160, 165);
  }
}

void ProcessStates(bool newGame, bool capture, byte toggleValues)
{
  if (newGame)
  {
    game.state = State::NewGameSetup;
  }
  else if (game.state == State::Hold)
  {
    // Do nothing until new game.
  }
  else if (game.state == State::NewGameSetup)
  {
    game.turn = 1;
    game.scoreTotal = 0;
    game.showDecValues = !digitalRead(PIN_TOGGLE_DEC_HEX);
    game.newGameCountDown = newGameCountDownStart;
    PlayTone(Tone::NewGame);
    UpdateDisplay(Display::SelectDifficulty);
    UpdateDisplay(Display::UpdateDifficulty);
    game.state = State::SelectDifficulty;
  }
  else if (game.state == State::SelectDifficulty)
  {
    if (toggleValues == 1)
      game.difficulty = Difficulty::Easy;
    if (toggleValues == 2)
      game.difficulty = Difficulty::Medium;
    if (toggleValues == 4)
      game.difficulty = Difficulty::Hard;

    static Difficulty previousDifficulty = Difficulty::Hard;
    if (previousDifficulty != game.difficulty)
    {
      previousDifficulty = game.difficulty;

      UpdateDisplay(Display::UpdateDifficulty);
    }

    if (capture)
    {
      game.state = State::ResetToggles;
      UpdateDisplay(Display::ResetTogglesOnly);
    }
  } 
  else if (game.state == State::Play)
  {
    UpdateDisplay(Display::Countdown);

    if (millis() - game.startTimeToCapture > timeMSAllowedToCapture)
    {
      game.score = 0;
      if (++game.turn > game.numTurns)
      {
        PlayTone(Tone::EndOfGame);
        UpdateDisplay(Display::FinalScore);
        game.state = State::EndOfGame;
      }
      else
      {
        PlayTone(Tone::CaptureFail);
        UpdateDisplay(Display::GameInfo);
        UpdateDisplay(Display::ResetTogglesWithScore);
        game.state = State::ResetToggles;
      }
    }

    if (capture)
    {
      if (toggleValues == game.target)
      {
        game.score = timeMSAllowedToCapture - (millis() - game.startTimeToCapture);
        game.scoreTotal += game.score;
        if (++game.turn > game.numTurns)
        {
          PlayTone(Tone::EndOfGame);
          UpdateDisplay(Display::FinalScore);
          game.state = State::EndOfGame;
        }
        else
        {
          PlayTone(Tone::CaptureSuccess);
          UpdateDisplay(Display::GameInfo);
          UpdateDisplay(Display::ResetTogglesWithScore);
          game.state = State::ResetToggles;
        }
      }
      else
      {
        PlayTone(Tone::CaptureFail);
      }
    }
  }
  else if (game.state == State::ResetToggles)
  {
    if (toggleValues == 0)
    {
      PlayTone(Tone::TogglesReset);
      game.score = 0;
      game.target = GenerateTarget(game.target, difficultyBitsMin[int(game.difficulty)], difficultyBitsMax[int(game.difficulty)]);
      game.startTimeToCapture = millis();
      UpdateDisplay(Display::GameInfo);
      UpdateDisplay(Display::Target);
      game.state = State::Play;
    }
  } 
}

byte GetToggleValues()
{
  byte toggleValues = 0;

  for (int n = 0; n < 8; n++)
  {
    bool toggleState = !sx1509.digitalRead(pins_sx1509_toggle[n]);
    bitWrite(toggleValues, n, toggleState);
  }

  return toggleValues;
}

void SetLEDs(byte value, bool blink)
{
  for (int n = 0; n < 8; n++)
  {
    int brightness = bitRead(value, n) ? ledMaxBrightnessPWMValue : 0;
    sx1509.analogWrite(pins_sx1509_led[n], brightness);
  }
}

void SetSegmentDisplays(byte toggleValues)
{
  uint8_t digits[] = {0, 0, 0, 0};
  displayDec.showNumberDec(toggleValues, false);
  digits[2] = displayHex.encodeDigit(toggleValues >> 4);
  digits[3] = displayHex.encodeDigit(toggleValues & 0x0F);
  displayHex.setSegments(digits);
}

void setup()
{
  delay(500);
  Serial.begin(115200);
  Serial.println("Binary Challenge");

  pinMode(PIN_BUTTON_CAPTURE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NEW_GAME, INPUT_PULLUP);
  pinMode(PIN_TOGGLE_DEC_HEX, INPUT_PULLUP);

  tft.init();
  delay(150);
  tft.setRotation(3);
  delay(150);
  UpdateDisplay(Display::Splash);
  PlayTone(Tone::Startup);

  if (sx1509.begin(SX1509_I2C_ADDRESS) == false)
  {
    Serial.println("Error: SX1509 failed to communicate.");
  }

  for (int i = 0; i < 8; i++)
  {
    sx1509.pinMode(pins_sx1509_led[i], ANALOG_OUTPUT);
    sx1509.analogWrite(pins_sx1509_led[i], 0);
    sx1509.pinMode(pins_sx1509_toggle[i], INPUT);
    sx1509.pinMode(pins_sx1509_toggle[i], INPUT_PULLUP);
  }

  displayDec.setBrightness(0x02);
  displayHex.setBrightness(0x02);
}

void loop()
{
  bool newGame = !digitalRead(PIN_BUTTON_NEW_GAME);
  bool capture = !digitalRead(PIN_BUTTON_CAPTURE);

  byte toggleValues = GetToggleValues();

  SetLEDs(toggleValues, game.state != State::ResetToggles);
  SetSegmentDisplays(toggleValues);

  ProcessStates(newGame, capture, toggleValues);
}