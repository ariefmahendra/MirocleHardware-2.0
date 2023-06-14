#include <Arduino.h>

#ifndef BUZZER
#define BUZZER 1

typedef enum
{
    BUZZER_MODE_ACTIVE_HIGH = true,
    BUZZER_MODE_ACTIVE_LOW = false
} buzzer_mode_t;

class Buzzer
{
private:
    uint8_t _myPin;
    bool _mode, _debug = false;
    long _beepStart, _beepStop, _default_beep_duration;
    void soundOn();
    void soundOff();

public:
    Buzzer(uint8_t buzzerPin, bool buzzerMode, long default_beep_duration);
    ~Buzzer();
    void beep();
    void beepFor(long duration);
    void runner();
    void setDebug(bool debug);
};

Buzzer::Buzzer(uint8_t buzzerPin, bool buzzerMode, long default_beep_duration)
{
    _mode = buzzerMode;
    _myPin = buzzerPin;
    _default_beep_duration = default_beep_duration;

    pinMode(_myPin, OUTPUT);
    _beepStart = 0;
    _beepStop = 0;
}

Buzzer::~Buzzer() {}

void Buzzer::beep()
{
    _beepStart = millis();
    _beepStop = _beepStart + _default_beep_duration;
}
void Buzzer::beepFor(long duration)
{
    _beepStart = millis();
    _beepStop = _beepStart + duration;
}
void Buzzer::runner()
{
    if (_debug)
        Serial.println("buz run");
    if (_beepStop > millis())
    {
        soundOn();
    }
    else
    {
        soundOff();
    }
}
void Buzzer::soundOn()
{

    digitalWrite(_myPin, _mode ? HIGH : LOW);
}
void Buzzer::soundOff()
{
    digitalWrite(_myPin, _mode ? LOW : HIGH);
}
void Buzzer::setDebug(bool debug) { this->_debug = debug; }
#endif