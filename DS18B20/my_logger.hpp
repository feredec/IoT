#pragma once

#include <Arduino.h>
#include <string>

inline bool debug = true;

void SerialLog(const char* msg);
void SerialLog(const std::string& msg);
