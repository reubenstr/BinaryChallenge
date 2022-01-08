#include <Arduino.h>

int countBits(unsigned char byte);
byte GenerateTarget(byte previousTarget, int minNumBits, int maxNumBits);