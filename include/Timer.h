#ifndef TIMER_H
#define TIMER_H
#include <windows.h>

class Timer{
public:
    LONGLONG frequency;
    LONGLONG start_time;
    LONGLONG end_time;

    Timer();
    LONGLONG GetTime();
    LONGLONG Start();
    LONGLONG Stop();
    float Duration();
    void Advance();
};

#endif // TIMER_H
