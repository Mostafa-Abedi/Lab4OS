
#ifndef UTILITY_H_
#define UTILITY_H_
#include "queue.h"


#define MEMORY 1024


extern void init_mem(resources *res);




extern int alloc_mem(int size, int reserve, resources *res);


extern int alloc_resources(resources *res, process proc);




extern void free_mem(resources *res, int index, int size);



extern void clear_mem(resources *res);


extern void reset_resources(resources *res);


extern void free_resources(resources *res, process proc);



extern void load_dispatch(char *dispatch_file, node_t *queue, process tempProc);


#endif
