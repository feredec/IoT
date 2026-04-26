#pragma once

#include <SensirionI2cSht4x.h>
#include <Wire.h>
#include "my_logger.hpp"
#include "smHistory.hpp"

enum class Sm_Sht40 : unsigned {
    RunOnce,
    Idle,
    Measure,
    // WaitForConversion,
    // Process,
    Output,
    Error
};


class MySht40 {

    public:
    MySht40(const uint8_t addr_, const uint8_t i2c_sda_, const uint8_t i2c_scl_, const unsigned long idleTime_) 
    : 
    addr(addr_),
    i2c_sda(i2c_sda_),
    i2c_scl(i2c_scl_),
    idleTime(idleTime_)
    {
        this->currentMillis = 0;
        this->sm.step       = Sm_Sht40::RunOnce;
        this->aTemperatureC = 0.0;
        this->aTemperatureF = 0.0;
        this->aHumidity     = 0.0;
    
    };


    const int16_t NO_ERROR = 0;
    int16_t error, errorOld;
    char errorMessage[64];

    SensirionI2cSht4x sensor;

    float aTemperatureC;
    float aTemperatureF;
    float aHumidity;

    void setup();
    void loop();


    private:
    uint8_t addr;
    uint8_t i2c_sda, i2c_scl;

    unsigned long idleTime;             // idle time between requests

    unsigned long startingMillis;
    unsigned long currentMillis;
    unsigned long conversionStartMillis;

    uint16_t conversionTime;
    SmHistory<Sm_Sht40> sm;

    char buf[80];
};
