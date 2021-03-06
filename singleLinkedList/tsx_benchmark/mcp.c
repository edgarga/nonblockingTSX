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
int standard;
int pop;
int pop_back;
int queue;
int useTsxSearch;
int nonBlock;
int tsxTries;

static struct timespec timer_begin;
static struct timespec timer_end;
static struct timespec timer_sum = {0, 0};

void help(char *p) {
    printf("Usage: %s [OPTION]...\n\n", p);
    printf("options:\n");
    printf("  -n <NUM>    input size\n");
    printf("  -t <NUM>    run <NUM> threads\n");
    printf(" [-tsxTries <NUM>]   number of TSX tries before using nonBlocking method\n");
    printf(" [-standard]  runs standard test\n");
    printf(" [-nonBlock]  runs the test only in nonBlocking mode (no TSX operations); same as -tsxTries 0\n");
    printf(" [-stackFront]  inserts and deletes -n times always on the front of the list with -t threads popping  and -t pushing\n");
    printf(" [-stackBack]  inserts and deletes -n times always on the back of the list with -t threads popping and -t pushing\n");
    printf(" [-queue]     inserts -n times with -t threads on the front and deletes -n times with -t threads on the back\n");
    printf(" [-packed]    inserts and deletes -n times random numbers between 0 - 9 with -t threads inserting and -t deleting\n");


    exit(1);
}

void mcp_init(int argc, char **argv) {
    int i;

    num_elements = 0;
    num_threads = 0;
    doShort = 0;
    focusBack = 0;
    workbench = 0;
    standard = 0;
    queue = 0;
    useTsxSearch = 0;
    nonBlock = 0;
    tsxTries = -1;


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
        } else if (strncmp(argv[i], "-tsxTries", 3) == 0) {
            if (i + 1 < argc) {
                tsxTries = atoi(argv[i + 1]);
                i++;
            }
        } else if (strncmp(argv[i], "-packed", 3) == 0) {
            doShort = 1;
        } else if (strncmp(argv[i], "-focusBack", 3) == 0) {
            focusBack = 1;
        } else if (strncmp(argv[i], "-focusFront", 3) == 0) {
            focusFront = 1;
        } else if (strncmp(argv[i], "-workbench", 3) == 0) {
            workbench = 1;
        } else if (strncmp(argv[i], "-nonBlock", 3) == 0) {
            nonBlock = 1;
        } else if (strncmp(argv[i], "-standard", 3) == 0) {
            standard = 1;
        } else if (strncmp(argv[i], "-stackFront", 5) == 0) {
            pop = 1;
        } else if (strncmp(argv[i], "-stackBack", 5) == 0) {
            pop_back = 1;
        } else if (strncmp(argv[i], "-queue", 3) == 0) {
            queue = 1;
        } else if (strncmp(argv[i], "-useTsxSearch", 3) == 0) {
            useTsxSearch = 1;
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
    printf("\n *** EXECUTION TIME: %.9lf s ***\n\n", s);
}

int get_num_threads() {
    return num_threads;
}

int get_num_elements() {
    return num_elements;
}

