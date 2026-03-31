#include "my_logger.hpp"

void SerialLog(const char* msg) {
  if (debug) {
    Serial.print(msg);
  }
}

void SerialLog(const std::string& msg) {
  if (debug) {
    SerialLog (msg.c_str());
  } 
}

void SerialLog_LedState(const char* state, unsigned long currMillis) {
  if (!debug) return;

  char buffer[32];
  //const char* state = (newLedState == LED_ON) ? "\nON: " : "\nOFF: ";
  int n = snprintf(buffer, sizeof(buffer), "%-5s:%8lu", state, currMillis);
	if (n >= int(sizeof(buffer))) n = sizeof(buffer)-1;  // truncate
	buffer[n] = '\0';

  SerialLog(buffer);
}

