#include <Arduino.h>
#include <TaskTimer.h>

TaskTimer::TaskTimer(void (*callback)(void), unsigned long interval, bool runOnce)
{
    this->_callback = callback;
    this->_interval = interval;
    this->_runOnce = runOnce;
}

void TaskTimer::Execute()
{
    if (this->_callback != NULL)
    {
        this->_callback();
    }

    if (!_runOnce)
    {
        this->ResetTimer();
    }
    else
    {
        this->Deactivate();
    }
}

void TaskTimer::ResetTimer()
{
    this->_lastRun = millis();
    this->_nextRun = this->_lastRun + this->_interval;
}

void TaskTimer::Tick()
{
    if (this->_enabled && this->_nextRun <= millis())
    {
        this->Execute();
    }
}

void TaskTimer::Activate(bool resetTimer)
{
    if (resetTimer)
        this->ResetTimer();
    this->_enabled = true;
}

void TaskTimer::Deactivate()
{
    this->_enabled = false;
}