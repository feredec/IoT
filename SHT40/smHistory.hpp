#pragma once

#include <Arduino.h>
#include "my_logger.hpp"

const size_t SM_BUF_LEN = 64;
const size_t SM_HIST_SIZE = 40;

/**
 * Generic Shift register - Forward
 * Pushes data forward, the oldest value gets lost.
 * Index [0] of the array receives the newVal
 * Be newVal a structured datatype, a temporary object my be needed.
 */
template <typename T>
void shiftRegF(T pArr[], size_t len, const T& newVal) {
	if (len > 1) {
		memmove((pArr + 1), (pArr), (len - 1) * sizeof(T));
	}
	*pArr = newVal;
}

/**
 * Generic Shift register - Forward
 * Pushes data forward, oldest value gets lost.
 * New value to be written directly to [0] after call, 
 * ie. without temp. object.
 */
template <typename T>
void shiftRegF(T pArr[], size_t len) {
	if (len > 1) {
		memmove((pArr + 1), (pArr), (len - 1) * sizeof(T));
	}
}


template <typename S>
struct tStateMachineHistoryItem {
    // time may be integrated in some form
    S        step;                       // Enum class
    char    name[SM_BUF_LEN];
    uint32  cycleCnt;
};


template <typename S>
struct SmHistory {
    S step;
    S prev;
    unsigned long cnt;
    tStateMachineHistoryItem<S> history[SM_HIST_SIZE];
    
    char buf[SM_BUF_LEN];
    
    void SaveHistory(const char* stepName) {

        if (step != history[0].step) {

            shiftRegF<tStateMachineHistoryItem<S>>(history, SM_HIST_SIZE);

            int n = snprintf(buf, sizeof(buf), "%s", stepName);
            if (n >= int(sizeof(buf))) n = sizeof(buf)-1;  // truncate if needed
            buf[n] = '\0';

            history[0].step = step;
            std::strcpy(history[0].name, buf);
        }

        ++history[0].cycleCnt;      // Increment this also when we stay in the same case 
    }
    

    void SaveHistory(const char* stepName, size_t optionalNum) {

        int n = snprintf(buf, sizeof(buf), "%s%u", stepName, optionalNum);
        if (n >= int(sizeof(buf))) n = sizeof(buf)-1;  // truncate if needed
        buf[n] = '\0';
        
        SaveHistory(buf);
    }

    void ShowStep() {
        int n = snprintf(buf, sizeof(buf), "%s:%u\n", history[0].name, history[0].cycleCnt);
        if (n >= int(sizeof(buf))) n = sizeof(buf)-1;  // truncate if needed
        buf[n] = '\0';
        SerialLog(buf);
    }

};



