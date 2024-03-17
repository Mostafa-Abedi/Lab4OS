
#ifndef QUEUE_H_
#define QUEUE_H_

#define MEMORY 1024


typedef struct {
    int printers;
    int scanners;
    int modems;
    int cds;
    int *mem_avail[MEMORY];
} resources;





typedef struct {
    int arrivalTime;
    int priority;
    int processTime;
    int memBytes;
    int printers;
    int scanners;
    int modems;
    int cds;
    int pid;
    int memAddress;
    int allocated;
    int paused;
} process;



typedef struct node{
    process proc;
    struct node *next;
} node_t;





extern void push(node_t *queue, process proc);


extern process pop(node_t *queue);


#endif
