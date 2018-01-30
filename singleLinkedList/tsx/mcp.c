#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int num_elements;
int num_threads;
int doShort;
int focusBack;
int focusFront;
int workbench;
int stack;
int pop;
int pop_back;
int queue;
int queueSize;
int nonBlock;

static struct timespec timer_begin;
static struct timespec timer_end;
static struct timespec timer_sum = {0, 0};

void help(char *p) {
    printf("Usage: %s [OPTION]...\n\n", p);
    printf("options:\n");
    printf("  -n <NUM>    input size\n");
    printf("  -t <NUM>    run <NUM> threads\n");
    printf(" [-intense]   runs intense test with -n random numbers in the range 0 - 9\n");
    printf(" [-standard]  runs standard test\n");

    exit(1);
}

void mcp_init(int argc, char **argv) {
    int i;

    num_elements = 0;
    num_threads = 0;
    doShort = 0;
    focusBack = 0;
    workbench = 0;
    stack = 0;
    queue = 0;
    queueSize = 0;
    nonBlock = 0;


    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-n", 3) == 0) {
            if (i + 1 < argc) {
                num_elements = atoi(argv[i + 1]);
                i++;
            }
        } else if (strncmp(argv[i], "-t", 3) == 0) {
            if (i + 1 < argc) {
                num_threads = atoi(argv[i + 1]);
                i++;
            }
        } else if (strncmp(argv[i], "-intense", 3) == 0) {
            doShort = 1;
        } else if (strncmp(argv[i], "-focusBack", 3) == 0) {
            focusBack = 1;
        } else if (strncmp(argv[i], "-focusFront", 3) == 0) {
            focusFront = 1;
        } else if (strncmp(argv[i], "-workbench", 3) == 0) {
            workbench = 1;
        } else if (strncmp(argv[i], "-nonBlock", 3) == 0) {
            nonBlock = 1;
        } else if (strncmp(argv[i], "-stack", 3) == 0) {
            stack = 1;
        } else if (strncmp(argv[i], "-popFront", 5) == 0) {
            pop = 1;
        } else if (strncmp(argv[i], "-popBack", 5) == 0) {
            pop_back = 1;
        } else if (strncmp(argv[i], "-queue", 3) == 0) {
            queue = 1;
            if (i + 1 < argc) {
                queueSize = atoi(argv[i + 1]);
                i++;
            }
        } else {
            help(argv[0]);
        }
    }

    if ((num_elements == 0) || (num_threads == 0)) {
        help(argv[0]);
    }

}

void time_start() {
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &timer_begin) != 0) {
        fprintf(stderr, "cannot get the time\n");
    }
}

void time_stop() {
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &timer_end) != 0) {
        fprintf(stderr, "cannot get the time\n");
        return;
    }
    if (timer_end.tv_nsec < timer_begin.tv_nsec) {
        timer_end.tv_sec = timer_end.tv_sec - timer_begin.tv_sec - 1;
        timer_end.tv_nsec = 1000000000 + timer_end.tv_nsec - timer_begin.tv_nsec;
    } else {
        timer_end.tv_sec = timer_end.tv_sec - timer_begin.tv_sec;
        timer_end.tv_nsec = timer_end.tv_nsec - timer_begin.tv_nsec;
    }
    timer_sum.tv_sec += timer_end.tv_sec;
    timer_sum.tv_nsec += timer_end.tv_nsec;
}

double time_get() {
    double s;
    timer_sum.tv_sec += timer_sum.tv_nsec / 1000000000;
    timer_sum.tv_nsec = timer_sum.tv_nsec % 1000000000;
    s = timer_sum.tv_sec + ((double) timer_sum.tv_nsec / 1000000000.0);
    return s;
}

void time_print() {
    double s;

    s = time_get();
    printf("\n *** EXECUTION TIME: %.9lfs ***\n\n", s);
}

int get_num_threads() {
    return num_threads;
}

int get_num_elements() {
    return num_elements;
}

