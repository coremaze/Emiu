#ifndef TIMER_H
#define TIMER_H
#include "types.h"

#ifdef WIN32
#include <windows.h>
#endif

class Timer {
private:
    i64 start_time;
    i64 end_time;
    #ifdef WIN32
    i64 frequency;
    #endif
public:
    Timer();
    i64 GetTime();
    i64 Start();
    i64 Stop();
    float Duration();
    void Advance();

    static void USleep(i32 ms);
};

#endif // TIMER_H
