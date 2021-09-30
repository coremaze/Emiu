#include "Timer.h"

#ifndef WIN32
#include <time.h>
#include <unistd.h>
#endif

Timer::Timer(){
    #ifdef WIN32
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    this->frequency = freq.QuadPart;
    #endif
}

i64 Timer::GetTime(){
    #ifdef WIN32
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time.QuadPart;
    #else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    i64 nanos = (i64)now.tv_sec * (i64)1e9 + (i64)now.tv_nsec;
    return nanos;
    #endif
}

i64 Timer::Start(){
    this->start_time = this->GetTime();
    return start_time;
}

i64 Timer::Stop(){
    this->end_time = this->GetTime();
    return end_time;
}

float Timer::Duration(){
    if (this->end_time > this->start_time)  {
        #ifdef WIN32
        return ((((float)this->end_time) - ((float)this->start_time)) * 1000.0) / ((float)frequency);
        #else
        return ((float)end_time - (float)start_time) / 1e6;
        #endif
    }
    else return 0.0;
}

void Timer::Advance(){
    this->start_time = this->end_time;
}
#include <stdio.h>
void Timer::USleep(i32 ms) {
    #ifdef WIN32
    Sleep(ms);
    #else
    usleep(ms * 1000);
    #endif
}