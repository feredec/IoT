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



