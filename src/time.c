#include "time.h"

Time get_time(void)
{
    return clock();
}

double time_elapsed_seconds(Time time)
{
    return ((double)(get_time() - time)) / CLOCKS_PER_SEC;
}

double time_elapsed_mseconds(Time time)
{
    return time_elapsed_seconds(time) * 1000;
}

void time_print_elapsed_seconds(Time time)
{
    printf("%lfs\n", time_elapsed_seconds(time));
}

void time_print_elapsed_mseconds(Time time)
{
    printf("%lfms\n", time_elapsed_mseconds(time));
}
