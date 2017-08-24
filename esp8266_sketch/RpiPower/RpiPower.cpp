/*
  RpiPower.cpp - Library defining a Raspberry Pi power connection
  Created by Scott Webster Wood, August 7, 2016
  Released into the public domain.
*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "RpiPower.h"

RpiPower::RpiPower(uint8_t pin, uint8_t id1, uint8_t id2, char* name)
{
  initRelay(pin, false);
  _idc1 = id1;
  _idc2 = id2;
  strncpy(_name, name, 8);
}
void RpiPower::setIdc1(uint8_t d) {
        _idc1 = d;
}
uint8_t RpiPower::getIdc1()
{
        return _idc1;
}
void RpiPower::setIdc2(uint8_t d)
{
        _idc2 = d;
}
uint8_t RpiPower::getIdc2()
{
        return _idc2;
}

void RpiPower::setName(char* n) {
        strncpy(_name, n, 8);
}
void RpiPower::copyName(char* n) {
        strncpy(n, _name, 8);
}
uint8_t RpiPower::getStatusDigit() {
        return 0b01100011;
        if(_relayState == true) {
                return SEG_DIP_ON;
        } else {
                return SEG_DIP_OFF;
        }
}
// relay related methods
void RpiPower::initRelay(uint8_t rPin, bool rState)
{
  _pin = rPin;
  pinMode(rPin, OUTPUT);
  if (rState != 0) {
          turnRelayOn();
  } else {
          turnRelayOff();
  }
}
void RpiPower::turnRelayOn()
{
        digitalWrite(_pin, LOW);
        _relayState = true;
}
void RpiPower::turnRelayOff()
{
        digitalWrite(_pin, HIGH);
        _relayState = false;
}
void RpiPower::toggleRelay()
{
        if(_relayState == true) {
                turnRelayOff();
        } else {
                turnRelayOn();
        }
}
bool RpiPower::getRelayState() { return _relayState; }
bool RpiPower::relayIsOn()     { return (_relayState == true); }
bool RpiPower::relayIsOff()    { return (_relayState == false); }
