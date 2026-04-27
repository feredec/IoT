## Sensirion SHT40 implementation class features:
- ESP8266 / ESP32
- processor run: non-blocking class
- uses Wire and SensirionI2cSht4x libraries
    - Wire by Arduino (built-in)
    - Sensirion I2C SHT4x by Sensirion (v1.1.2)
- VSCode + PlatformIO files location within project:
    - /include/my_sht40.hpp
    - /src/my_sht40.cpp


## Usage:
_main.cpp:_
    
    #include "my_SHT40.hpp"
    
    //I2C
    constexpr uint8_t I2C_ADDR = 0x44;
    constexpr uint8_t I2C_BUS_SDA_PIN = 4;                // GPIO4 = D2
    constexpr uint8_t I2C_BUS_SCL_PIN = 5;                // GPIO5 = D1
    constexpr unsigned READ_INTERVAL = 5000;
    
    MySht40 sht40(I2C_ADDR, I2C_BUS_SDA_PIN, I2C_BUS_SCL_PIN, READ_INTERVAL);
    
    void setup() {
      sht40.setup();
    }
    
    void loop() {
      sht40.loop();
    }


## Implementation notes:
- The class does not block processor by calling delay(), lets conversion take its time instead.
- Time differences evaluated using millis().
- During idle time the processor is free to handle other sensors, web server etc.
- optional (integrated) state machine logging via SmHistory struct.
