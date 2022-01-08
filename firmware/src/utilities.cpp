#include <Arduino.h>

int countBits(unsigned char byte)
{
    int count = 0;
    for (int i = 0; i < 8; i++)
        count += (byte >> i) & 0x01;
    return count;
}

byte GenerateTarget(byte previousTarget, int minNumBits, int maxNumBits)
{
    byte target;

    do
    {
        target = random(0, 256);
    } while ((countBits(target) < minNumBits || countBits(target) > maxNumBits) && target == previousTarget);

    return target;
}