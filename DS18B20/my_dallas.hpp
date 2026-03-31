#pragma once

#include "OneWire.h"
#include "DallasTemperature.h"
#include "my_logger.hpp"

constexpr unsigned MAX_SENSORS = 8;


typedef uint8 tAddr[8];                 // Address of a DS sensor consists of 8 bytes

enum class SmDiscovery : unsigned {
    RunOnce,
    Discover,
    Wait,
    Idle
};

enum class SmLoop : unsigned {
    RunOnce,
    Idle,
    Request,
    WaitForConversion,
    Process
};

struct tResult {
    float tempC;
    float tempF;
};

class MyDs18b20 {

    public:
    MyDs18b20(const int pin, const unsigned long discoveryWaitTime, const unsigned long idleTime);

    const unsigned getSensorCount() const;
    const float getTempC(unsigned index) const;
    const float getTempF(unsigned index) const;
    const tResult* getResult(unsigned index) const;

    void setup();
    void loop_Discovery();
    void loop_OneSensor();
    void loop_AllSensors();

    private:
    unsigned long idleTime;             // idle time between requests
    unsigned long discoveryWaitTime;    // wait time between discovery attempts

    unsigned long startingMillis;
    unsigned long currentMillis;
    unsigned long conversionStartMillis;
    unsigned long discoveryWaitMillis;

    unsigned sensorCount;
    uint16_t conversionTime;
    SmLoop smLoop;
    SmDiscovery smDiscovery;

    tAddr found[MAX_SENSORS];           // search() result to store in

    tResult results[MAX_SENSORS];

    OneWire ow;
    DallasTemperature sensors;

    void addrToString(const tAddr addr, char* out, size_t len);
    bool isSupported(uint8_t family);

};
