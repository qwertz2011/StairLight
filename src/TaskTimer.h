#ifndef TaskTimer_h
#define TaskTimer_h

#include <Arduino.h>

class TaskTimer
{
protected:
    bool _enabled = false;
    bool _runOnce = false;
    void (*_callback)(void);
    unsigned long _lastRun = 0;
    unsigned long _nextRun = 0;
    unsigned long _interval = 1000;

    void Execute();

public:
    TaskTimer(void (*callback)(void) = NULL, unsigned long interval = 0, bool runOnce = false);
    void Tick();
    void Deactivate();
    void Activate(bool resetTimer = false);
    void ResetTimer();
};

#endif