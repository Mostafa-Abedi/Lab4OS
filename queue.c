
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"




void push(node_t *queue, process proc){
    node_t *cur = queue;
    
    while(cur->next != NULL){
        cur = cur->next;
    }
    
    cur->next = (node_t*)malloc(sizeof(node_t));
    
    cur->next->proc = proc;
    
    cur->next->next = NULL;    
}

process pop(node_t *queue){
    
    if(queue->next == NULL){
        exit(0);
    }
    
    process popped = queue->next->proc;
    node_t *temp = queue->next; 
    
    free(temp);
    
    queue->next= queue->next->next;
    return popped;
}
