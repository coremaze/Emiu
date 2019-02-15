#include "Timer.h"

Timer::Timer(){
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    this->frequency = freq.QuadPart;
}

LONGLONG Timer::GetTime(){
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time.QuadPart;
}

LONGLONG Timer::Start(){
    this->start_time = this->GetTime();
    return start_time;
}

LONGLONG Timer::Stop(){
    this->end_time = this->GetTime();
    return end_time;
}

float Timer::Duration(){
    if (this->end_time > this->start_time)  return ((((float)this->end_time) - ((float)this->start_time)) * 1000.0) / ((float)frequency);
    else return 0.0;
}

void Timer::Advance(){
    this->start_time = this->end_time;
}
