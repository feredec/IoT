
#include "my_dallas.hpp"

class MyDs18b20;

MyDs18b20::MyDs18b20(const int pin, const unsigned long discoveryWaitTime, const unsigned long idleTime) 
: 
ow(pin), sensors(&ow)
{
    this->sensorCount        = 0;
    this->idleTime           = idleTime;
    this->discoveryWaitTime  = discoveryWaitTime; 
    this->currentMillis      = 0;
    this->smLoop             = SmLoop::RunOnce;
}

const unsigned MyDs18b20::getSensorCount() const {
    return sensorCount;
}

const float MyDs18b20::getTempC(unsigned index) const {
    if (index < sensorCount) return results[index].tempC;
    return -300.0; // invalid index
}

const float MyDs18b20::getTempF(unsigned index) const {
    if (index < sensorCount) return results[index].tempF;
    return -300.0; // invalid index
}

const tResult* MyDs18b20::getResult(unsigned index) const {
    if (index < sensorCount) return &results[index];
    return nullptr; // invalid index
}

void MyDs18b20::setup() {
    sensors.begin();
    sensors.setWaitForConversion(false);        // Disable delay() in requestTemperatures(), and thus processor blocking
    
    conversionTime = sensors.millisToWaitForConversion(
        sensors.getResolution()
    );
    idleTime -= conversionTime;                 // Wait less by the processing time.
    startingMillis = millis() - idleTime;
}


void MyDs18b20::loop_OneSensor() {

    currentMillis = millis();

    switch (smLoop) {

        case SmLoop::RunOnce:
            smLoop = SmLoop::Request;
        break;

        case SmLoop::Idle:
            if (currentMillis - startingMillis >= idleTime) {           
                smLoop = SmLoop::Request;
            }
        break;
            
        case SmLoop::Request:
            sensors.requestTemperatures();

            conversionStartMillis = currentMillis;
            smLoop = SmLoop::WaitForConversion;
        break;
            
        case SmLoop::WaitForConversion:
          if (currentMillis - conversionStartMillis >= conversionTime) {
                smLoop = SmLoop::Process;
            }
        break;
        
        case SmLoop::Process:
            {
                results[0].tempC = sensors.getTempCByIndex(0);
                results[0].tempF = sensors.getTempFByIndex(0);
        
                [this](tResult& r) {
                    char buffer[48];
                    unsigned long deltaT = currentMillis - startingMillis;
                    int n = snprintf(buffer, sizeof(buffer), "\ndT: +%ums -> Temp: %.2fºC / %.1fF", unsigned(deltaT), r.tempC, r.tempF);
                    if (n >= int(sizeof(buffer))) n = sizeof(buffer)-1;  // truncate
                    buffer[n] = '\0';
            
                    SerialLog(buffer);
                }(results[0]);

                startingMillis += idleTime;     // to pre precisely deterministic without drift , ie. 10 - 20 - 30 - 40, not 10 - 20.01 - 30.01 - 40.05
                smLoop = SmLoop::Idle;
            }
        break;
        
        default:
            smLoop = SmLoop::Idle;
        break;
   }

}


void MyDs18b20::addrToString(const tAddr addr, char* out, size_t len) {
    size_t n = 0;
    for (uint8_t i = 0; i < 8; i++) {
        n += snprintf(out + n, len - n, "%02X ", addr[i]);
    }
    if (n > 0) out[n - 1] = '\0';
}


bool MyDs18b20::isSupported(uint8_t family) {
    return (
        (family == DS18S20MODEL) || 
        (family == DS18B20MODEL) || 
        (family == DS1822MODEL) || 
        (family == DS1825MODEL) || 
        (family == DS28EA00MODEL)
    );
}


void MyDs18b20::loop_Discovery() {

    currentMillis = millis();
    tAddr discovered;

    switch (smDiscovery) {

        case SmDiscovery::RunOnce:
            ow.reset_search();                      // reset always done here before discovery
            smDiscovery = SmDiscovery::Discover;
            sensorCount = 0;
        break;

        case SmDiscovery::Discover:
            if ((sensorCount < MAX_SENSORS) && ow.search(discovered)) {
                
                char addrStr[32];
                addrToString(discovered, addrStr, sizeof(addrStr));

                const char* validStr;
                if ((ow.crc8(discovered, 7) != discovered[7]) || (!isSupported(discovered[0]))) {
                    validStr = "Invalid";
                } else {
                    validStr = "Valid";
                    memcpy(&found[sensorCount], discovered, sizeof(tAddr));
                    ++sensorCount;
                }

                char buffer[64];
                snprintf(buffer, sizeof(buffer), "\nAddr: %s  <- %s\n", addrStr, validStr);

                SerialLog(buffer);

                discoveryWaitMillis = currentMillis;
                smDiscovery = SmDiscovery::Wait;
            } else {
                SerialLog("No more sensors.\n");
                smDiscovery = SmDiscovery::Idle;
            }
        break;

        case SmDiscovery::Wait:
            if (currentMillis - discoveryWaitMillis >= discoveryWaitTime) {           
                smDiscovery = SmDiscovery::Discover;
            }
        break;

        case SmDiscovery::Idle:
            // Do nothing here till next discovery.
        break;
        
        default:

        break;
    }
}



void MyDs18b20::loop_AllSensors() {

    currentMillis = millis();

    switch (smLoop) {

        case SmLoop::RunOnce:
            smLoop = SmLoop::Request;
        break;

        case SmLoop::Idle:
            if (currentMillis - startingMillis >= idleTime) {           
                smLoop = SmLoop::Request;
            }
        break;
            
        case SmLoop::Request:
            sensors.requestTemperatures();

            conversionStartMillis = currentMillis;
            smLoop = SmLoop::WaitForConversion;
        break;
            
        case SmLoop::WaitForConversion:
          if (currentMillis - conversionStartMillis >= conversionTime) {
                smLoop = SmLoop::Process;
            }
        break;
        
        case SmLoop::Process:
            for (unsigned i = 0; i < sensorCount; i++) {
                results[i].tempC = sensors.getTempC(found[i]);
                results[i].tempF = sensors.getTempF(found[i]);
        
                [this](tResult& r) {
                    char buffer[48];
                    unsigned long deltaT = currentMillis - startingMillis;
                    int n = snprintf(buffer, sizeof(buffer), "\ndT: +%ums -> Temp: %.2fºC / %.1fºF", unsigned(deltaT), r.tempC, r.tempF);
                    if (n >= int(sizeof(buffer))) n = sizeof(buffer)-1;  // truncate
                    buffer[n] = '\0';
            
                    SerialLog(buffer);
                }(results[i]);
            }

            startingMillis += idleTime;     // to pre precisely deterministic without drift , ie. 10 - 20 - 30 - 40, not 10 - 20.01 - 30.01 - 40.05
            smLoop = SmLoop::Idle;
        break;
        
        default:
            smLoop = SmLoop::Idle;
        break;
   }

}

