#ifndef __MOTOR_H_
#define __MOTOR_H_

// header pentru subansamblul MOTOR

class MOTOR
{
private:
    int _m11, _m12, _m21, _m22;

public:
    MOTOR(int m11, int m12, int m21, int m22);
    void setup();
    void inainte();
    void inapoi();
    void stop();
    void dreapta();
    void stanga();
};

#endif