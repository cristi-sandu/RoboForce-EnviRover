#ifndef __SU_H_
#define __SU_H_

// header pentru subansamblul SU

#define PULSE_TIMEOUT 150000L // 100ms

class SU
{
private:
    int _echoPin, _triggerPin;
    long microsecondsToCentimeters(long microseconds);
    long durata();
    long durataMediana();

public:
    SU(int echoPin, int triggerPin);
    long distanta();
};

#endif