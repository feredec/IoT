#pragma once

#include <Arduino.h>

#include <string>

inline bool debug = true;
inline bool resumeLedStateMonitor = false;

void SerialLog(const char* msg);
void SerialLog(const std::string& msg);

void SerialLog_LedState(const char* state, unsigned long currMillis);