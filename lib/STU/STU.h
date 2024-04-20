#ifndef __STU_H_
#define __STU_H_

#define MT int
#define BITS 40

class STU
{
private:
    int _pin;
    byte bits2byte(byte data[8]);
    bool confirm(int us, byte level);

public:
    STU(int pin);
    byte readRHT(MT *rh, MT *t);
    void begin();
};

#endif