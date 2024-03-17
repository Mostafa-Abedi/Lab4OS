
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "queue.h"

#define PRINTERS 2
#define SCANNERS 1
#define MODEMS 1
#define CDS 2

void init_mem(resources *res){
    
    for(int i=0; i<MEMORY; i++){
        res->mem_avail[i]=0;
    }
}


int alloc_mem(int size, int reserve, resources *res){
    int reserved, address;
    int available=0;

    reserved = reserve;

    
    for(int i = reserved+1; i < MEMORY && available<size; i++){
        
        if(res->mem_avail[i-1] == 0 && res->mem_avail[i] == 0 ){
            available++;
        }
        
    }

    
    if(size<=available){
        for(int i = reserved; i < MEMORY && available > 0; i++){
            
            if(res->mem_avail[i] == 0){
                
                if(available>0){
                    res->mem_avail[i]=1;
                    available--;
                    
                    address = (i-size)+1;
                }
            }
        }
    }else{
        address = -1;
    }

    return address;
}


int alloc_resources(resources *res, process proc){

    
    if(res->printers>0 && res->scanners >0 && res->modems>0 && res->cds>0){
        if((res->printers - proc.printers) < 0){
            return 0;
        }else{
            res->printers -=proc.printers;
        }

        if(res->scanners - proc.scanners < 0){
            return 0;
        }else{
            res->scanners -= proc.scanners;
        }

        if(res->modems - proc.modems < 0){
            return 0;
        }else{
            res->modems -= proc.modems;
        }

        if(res->cds - proc.cds < 0){
            return 0;
        }else{
            res->cds -= proc.cds;
        }

        return 1;
    }else{
        return 0;
    }

}


void clear_mem(resources *res){
    for(int i = 0; i < MEMORY; i++){
        res->mem_avail[i] = 0;
    }
}


void free_mem(resources *res, int index, int size){
    for(int i = index; i < size; i++){
        res->mem_avail[i] = 0;
    }
}


void reset_resources(resources *res){
    res->printers = 2;
    res->scanners = 1;
    res->modems = 1;
    res->cds = 2;
}


void free_resources(resources *res, process proc){
    res->printers += proc.printers;
    res->scanners += proc.scanners;
    res->modems += proc.modems;
    res->cds += proc.cds;
}



void load_dispatch(char *dispatch_file, node_t *queue, process tempProc){
    
    FILE *fp = fopen(dispatch_file,"r");
    char buff[256];
    char newline[2] = "\n";
    char comma[2] = ",";
    char *token;
    while(fgets(buff, 256, fp) != NULL){
        
        token = strtok(buff, newline);

        
        token = strtok(token, comma);
        tempProc.arrivalTime = atoi(token);
        token = strtok(NULL, comma);
        tempProc.priority = atoi(token);
        token = strtok(NULL, comma);
        tempProc.processTime = atoi(token);
        token = strtok(NULL, comma);
        tempProc.memBytes= atoi(token);
        token = strtok(NULL, comma);
        tempProc.printers = atoi(token);
        token = strtok(NULL, comma);
        tempProc.scanners = atoi(token);
        token = strtok(NULL, comma);
        tempProc.modems = atoi(token);
        token = strtok(NULL, comma);
        tempProc.cds = atoi(token);
        tempProc.pid = 0;
        tempProc.memAddress = -1;
        tempProc.allocated = 0;
        tempProc.paused = 0;

        
        push(queue, tempProc);
    }
    fclose(fp);
}
