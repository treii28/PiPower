/*
  RpiPower.h - Library defining a Raspberry Pi power connection
  Created by Scott Webster Wood, August 7, 2016
  Released into the public domain.
*/
#ifndef RpiPower_h
#define RpiPower_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define SEG_DIP_ON  0b01100011
#define SEG_DIP_OFF 0b01011100

class RpiPower
{
  public:
    RpiPower(uint8_t pin, uint8_t id1, uint8_t id2, char* name);
    void    setIdc1(uint8_t d);
        uint8_t getIdc1();
    void    setIdc2(uint8_t d);
        uint8_t getIdc2();
        void    setName(char* n);
        void    copyName(char* n);
        uint8_t getStatusDigit();

    void initRelay(uint8_t rPin, bool rState);
    void turnRelayOn();
        void turnRelayOff();
        void toggleRelay();
        bool getRelayState();
        bool relayIsOn();
        bool relayIsOff();

  private:
    uint8_t    _pin;      // relay pin
        uint8_t    _idc1;     // first character id to display
        uint8_t    _idc2;     // second character id to display
        char       _name[8];     // user friendly name
        bool       _relayState;
};
