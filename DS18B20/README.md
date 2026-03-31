## Dallas DS18B20 implementation class features:
- ESP8266 / ESP32
- processor run: non-blocking class
- uses OneWire and DallasTemperature libraries
- offers sensors discovery
- VSCode + PlatformIO files location within project:
    - /include/my_dallas.hpp
    - /src/my_dallas.cpp


## Usage:
_main.cpp:_

	#include "my_dallas.hpp"
	constexpr int ONE_WIRE_BUS_PIN    = 4;      // Change per need
	constexpr unsigned DISCOVERY_WAIT = 250;
	constexpr unsigned READ_INTERVAL = 5000;
	
	MyDs18b20 ds18b20(ONE_WIRE_BUS_PIN, DISCOVERY_WAIT, READ_INTERVAL);
	
	void setup() {
	  ds18b20.setup();
	}
	
	void loop() {
	  // One or multiple sensors using discovery
	  ds18b20.loop_Discovery();
	  ds18b20.loop_AllSensors();

	  // One sensor using indexing, ie. index 0.
	  // ds18b20.loop_OneSensor();
	}
	

## Implementation notes:
- The class does not block processor by calling delay(), lets conversion take its time instead.
- Lib DallasTemperature offers implementation freedom, see .setWaitForConversion(false).
- Time differences evaluated using millis().
- During idle time the processor is free to handle other sensors, web server etc.
- Idle time takes processing time into account => eg. reading each 5000ms - processing 750ms = idle 4250ms

