#include <Arduino.h>
#include "SU.h"

SU::SU(int echoPin, int triggerPin)
{
    _echoPin = echoPin;
    _triggerPin = triggerPin;
    pinMode(_echoPin, INPUT);
    pinMode(_triggerPin, OUTPUT);
}

long SU::microsecondsToCentimeters(long duration)
{
    return (duration * 100) / 5882;
}

long SU::durata()
{
    long duration = 0;

    digitalWrite(_triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_triggerPin, LOW);
    delayMicroseconds(2);
    duration = pulseIn(_echoPin, HIGH, PULSE_TIMEOUT);
    return duration;
}

long SU::durataMediana()
{
    long d[3];
    long t;

    d[0] = durata();
    delay(25);
    d[1] = durata();
    delay(25);
    d[2] = durata();

    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 2; i++)
        {
            if (d[i] > d[i + 1])
            {
                t = d[i + 1];
                d[i + 1] = d[i];
                d[i] = t;
            }
        }
    }
    return d[1];
}

long SU::distanta()
{
    long cm;

    // convert the time into a distance
    cm = microsecondsToCentimeters(durataMediana());
    return cm;
}
