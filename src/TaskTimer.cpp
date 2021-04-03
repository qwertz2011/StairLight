#include <Arduino.h>
#include <TaskTimer.h>

TaskTimer::TaskTimer(void (*callback)(void), unsigned long interval)
{
    this->_interval = interval;
    this->_callback = callback;
}

void TaskTimer::Run()
{
    if (this->_callback == NULL)
    {
        return;
    }
    this->_callback();
    this->ResetTimer();
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
        this->Run();
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