#include <Arduino.h>
#include "MOTOR.h"

MOTOR::MOTOR(int m11, int m12, int m21, int m22)
{
    _m11 = m11;
    _m12 = m12;
    _m21 = m21;
    _m22 = m22;
}

void MOTOR::setup()
{
    pinMode(_m11, OUTPUT);
    pinMode(_m12, OUTPUT);
    pinMode(_m21, OUTPUT);
    pinMode(_m22, OUTPUT);
    stop();
}

void MOTOR::stop()
{
    digitalWrite(_m11, LOW);
    digitalWrite(_m12, LOW);
    digitalWrite(_m21, LOW);
    digitalWrite(_m22, LOW);
}

void MOTOR::inainte()
{
    digitalWrite(_m11, LOW);
    digitalWrite(_m12, HIGH);
    digitalWrite(_m21, LOW);
    digitalWrite(_m22, HIGH);
}

void MOTOR::inapoi()
{
    digitalWrite(_m11, HIGH);
    digitalWrite(_m12, LOW);
    digitalWrite(_m21, HIGH);
    digitalWrite(_m22, LOW);
}

void MOTOR::stanga()
{
    digitalWrite(_m11, LOW);
    digitalWrite(_m12, HIGH);
    digitalWrite(_m21, HIGH);
    digitalWrite(_m22, LOW);
}

void MOTOR::dreapta()
{
    digitalWrite(_m11, HIGH);
    digitalWrite(_m12, LOW);
    digitalWrite(_m21, LOW);
    digitalWrite(_m22, HIGH);
}