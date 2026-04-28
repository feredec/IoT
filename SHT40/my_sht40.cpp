#include "my_sht40.hpp"

class MySht40;


void MySht40::setup() {

    Wire.begin(this->i2c_sda, this->i2c_scl); //

    sensor.begin(Wire, this->addr);

    sensor.softReset();
    delay(10);
    uint32_t serialNumber = 0;
    errorOld = 0;
    error = sensor.serialNumber(serialNumber);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute serialNumber(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("serialNumber: ");
    Serial.print(serialNumber);
    Serial.println();


    startingMillis = millis() - idleTime;
    sm.step = Sm_Sht40::RunOnce;
}


void MySht40::loop() {


    currentMillis = millis();

    switch (sm.step) {

        case Sm_Sht40::RunOnce:
            sm.step = Sm_Sht40::Measure;
            break;

        case Sm_Sht40::Idle:
            if (currentMillis - startingMillis >= idleTime) {           
                sm.step = Sm_Sht40::Measure;
            }
            break;
            
        case Sm_Sht40::Measure:
            sm.SaveHistory("Sm_Sht40::Measure");

            error = sensor.measureMediumPrecision(aTemperatureC, aHumidity);
            aTemperatureF = (aTemperatureC * 1.8) + 32;

            if (error == NO_ERROR) {
                sm.step = Sm_Sht40::Output;
            } else {
                sm.step = Sm_Sht40::Error;
            }
            break;
        
        case Sm_Sht40::Output:
            {
                unsigned long deltaT = currentMillis - startingMillis;
                int n = snprintf(buf, sizeof(buf), "\nSHT40   -- dT: +%lums -> Temp: %.2f°C / %.1f°F\t| Humidity: %.2f%%", 
                   deltaT, aTemperatureC, aTemperatureF, aHumidity
                );
                if (n >= int(sizeof(buf))) n = sizeof(buf)-1;  // truncate
                buf[n] = '\0';

                Serial.print(buf);

                startingMillis += idleTime;     // to pre precisely deterministic without drift , ie. 10 - 20 - 30 - 40, not 10 - 20.01 - 30.01 - 40.05
                sm.step = Sm_Sht40::Idle;
            }
            break;
        
        case Sm_Sht40::Error:
            if (error != errorOld) {
                Serial.print("Error trying to execute measureLowestPrecision(): ");
                errorToString(error, errorMessage, sizeof errorMessage);
                Serial.println(errorMessage);

                errorOld = error;
            }
            
            startingMillis += 2 * idleTime;     // to pre precisely deterministic without drift , ie. 10 - 20 - 30 - 40, not 10 - 20.01 - 30.01 - 40.05
            sm.step = Sm_Sht40::Idle;
            break;
            
        default: 
            sm.step = Sm_Sht40::Idle;
            
        break;
    }
    
}
