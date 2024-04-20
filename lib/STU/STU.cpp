#include <Arduino.h>
#include "STU.h"

STU::STU(int pin)
{
    _pin = pin;
}

void STU::begin()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
}

byte STU::bits2byte(byte data[8])
{
    byte v = 0;

    for (int i = 0; i < 8; i++)
    {
        v += data[i] << (7 - i);
    }
    return v;
}

bool STU::confirm(int us, byte level)
{
    // wait one more count to ens ure.
    int cnt = us / 10 + 1;

    bool ok = false;
    for (int i = 0; i < cnt; i++)
    {
        if (digitalRead(_pin) != level)
        {
            ok = true;
            break;
        }
        delayMicroseconds(10);
    }
    return ok;
}

byte STU::readRHT(MT *rh, MT *t)
{
    byte bits[BITS];

    // notify DHT11 to start:
    //    1. PULL LOW 20ms.
    //    2. PULL HIGH 20-40us.
    //    3. SET TO INPUT.
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(30);
    pinMode(_pin, INPUT);

    // DHT11 starting:
    //    1. PULL LOW 80us
    //    2. PULL HIGH 80us
    if (!confirm(80, LOW))
    {
        return 100;
    }
    if (!confirm(80, HIGH))
    {
        return 101;
    }

    // DHT11 data transmite:
    //    1. 1bit start, PULL LOW 50us
    //    2. PULL HIGH 26-28us, bit(0)
    //    3. PULL HIGH 70us, bit(1)
    for (int j = 0; j < BITS; j++)
    {
        if (!confirm(50, LOW))
        {
            return 102;
        }

        // read a bit, should never call method,
        // for the method call use more than 20us,
        // so it maybe failed to detect the bit0.
        bool ok = false;
        int tick = 0;

        for (int i = 0; i < 8; i++, tick++)
        {
            if (digitalRead(_pin) != HIGH)
            {
                ok = true;
                break;
            }
            delayMicroseconds(10);
        }
        if (!ok)
        {
            return 103;
        }
        bits[j] = (tick > 3 ? 1 : 0);
    }

    // DHT11 EOF:
    //    1. PULL LOW 50us.
    if (!confirm(50, LOW))
    {
        return 104;
    }

    byte rh1, rh2, t1, t2, checkSum, expected;

    rh1 = bits2byte(&bits[0]);
    rh2 = bits2byte(&bits[8]);
    *rh = rh1;
    t1 = bits2byte(&bits[16]);
    t2 = bits2byte(&bits[24]);
    *t = t1;
    checkSum = bits2byte(&bits[32]);
    expected = (byte)(rh1 + rh2 + t1 + t2);

    if (checkSum != expected)
    {
        return 105;
    }
    else
    {
        return 0;
    }
}