#ifndef TIME_H
#define TIME_H

#include <time.h>

typedef clock_t Time;

Time get_time(void);

double time_elapsed_seconds(Time time);
double time_elapsed_mseconds(Time time);

void time_print_elapsed_seconds(Time time);
void time_print_elapsed_mseconds(Time time);

#endif
