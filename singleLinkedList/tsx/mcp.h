#ifndef MCP_H
#define MCP_H

#ifdef __cplusplus
extern "C" {
#endif

extern int num_elements;
extern int num_threads;
extern int doShort;
extern int standardTest;
extern int workbench;
extern int stack;
extern int stack_back;
extern int queue;
extern int pop;
extern int pop_back;
extern int queue;
extern int nonBlock;

void mcp_init(int argc, char **argv);

int get_num_threads();
int get_num_elements();

void time_start();
void time_stop();
double time_get();
void time_print();

#ifdef __cplusplus
}
#endif

#endif

