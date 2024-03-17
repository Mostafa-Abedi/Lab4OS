
#ifndef HOSTD_H_
#define HOSTD_H_


#include "queue.h"


extern void print_process(process proc);


extern void run_process(node_t * pullQueue, node_t * pushQueue, char *argv[]);

#endif
