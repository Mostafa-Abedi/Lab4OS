#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"
#include "utility.h"
#include "hostd.h"


#define MEMORY 1024
#define PRINTERS 2
#define SCANNERS 1
#define MODEMS 1
#define CDS 2


process job;
resources res;

node_t *jobQueue = NULL;
node_t * runtime = NULL;
node_t * priority1 = NULL;
node_t * priority2 = NULL;
node_t * priority3 = NULL;
node_t * tmpQueue = NULL;


void print_process(process proc);

int main(int argc, char *argv[]){
    res.printers= PRINTERS;
    res.scanners = SCANNERS;
    res.modems = MODEMS;
    res.cds = CDS;

    jobQueue = malloc(sizeof(node_t));
    runtime = malloc(sizeof(node_t));
    priority1 = malloc(sizeof(node_t));
    priority2 = malloc(sizeof(node_t));
    priority3 = malloc(sizeof(node_t));
    tmpQueue = malloc(sizeof(node_t));

    jobQueue->next = NULL;
    runtime->next = NULL;
    priority1->next = NULL;
    priority2->next = NULL;
    priority3->next = NULL;
    tmpQueue->next = NULL;

    load_dispatch("dispatchlist.txt", jobQueue, job);

    init_mem(&res);

    while(jobQueue->next != NULL){
        job = pop(jobQueue);
        if(job.priority ==3){
            push(priority3, job);
        }else if(job.priority >1){
            push(priority2, job);
        }else if(job.priority >0){
            push(priority1, job);
        }else{
            push(runtime, job);
        }
    }

    int address, allocated;
    process *tmpjob;

    
    while(runtime->next != NULL){
        tmpjob = &runtime->next->proc;

        
        if((address = alloc_mem(tmpjob->memBytes, 0, &res)) != -1){
            tmpjob->memAddress = address;
        }else{
            
            run_process(runtime, runtime, argv);
        }
        
        run_process(runtime, runtime, argv);
    }

    clear_mem(&res);
    while(priority1->next != NULL){
        tmpjob = &priority1->next->proc;

        address = alloc_mem(tmpjob->memBytes, 64, &res);
        allocated = alloc_resources(&res, *tmpjob);

        
        if(address != -1){
            tmpjob->memAddress = address;
            
            if(allocated != 0){
                tmpjob->allocated = allocated;

                
                push(tmpQueue,*tmpjob);
                pop(priority1);
            }else{
                
                while((allocated = alloc_resources(&res, *tmpjob)) == 0){
                    run_process(tmpQueue, priority2, argv);
                }
                
                tmpjob->allocated = allocated;

                
                push(tmpQueue,*tmpjob);
                pop(priority1);

                
                if(tmpQueue->next->next == NULL && priority1->next == NULL){
                    run_process(tmpQueue, priority2, argv);
                }
            }
        }else{
            run_process(tmpQueue, priority2, argv);
        }
    }

    clear_mem(&res);

    while(priority2->next != NULL){
        tmpjob = &priority2->next->proc;

        address = alloc_mem(tmpjob->memBytes, 64, &res);
        allocated = alloc_resources(&res, *tmpjob);

        
        if(address != -1){
            tmpjob->memAddress = address;
            
            if(allocated != 0){
                tmpjob->allocated = allocated;
                
                push(tmpQueue,*tmpjob);
                pop(priority2);
            }else{
                
                while((allocated = alloc_resources(&res, *tmpjob)) == 0){
                    run_process(tmpQueue, priority3, argv);
                }

                tmpjob->allocated = allocated;

                
                push(tmpQueue,*tmpjob);
                pop(priority2);


            }
            
            if(tmpQueue->next->next == NULL && priority2->next == NULL){
                
                run_process(tmpQueue, priority3, argv);
            }
        }else{
            if(tmpQueue->next->next != NULL){
                run_process(tmpQueue, priority3, argv);
            }else{
                push(priority2, pop(priority2));
                clear_mem(&res);
            }
        }
    }
    clear_mem(&res);
    while(priority3->next != NULL){
        tmpjob = &priority3->next->proc;

        address = alloc_mem(tmpjob->memBytes, 64, &res);
        allocated = alloc_resources(&res, *tmpjob);

        
        if(address != -1){
            tmpjob->memAddress = address;
            
            if(allocated != 0){
                tmpjob->allocated = allocated;
                
                push(tmpQueue,*tmpjob);
                pop(priority3);
            }else{
                
                while((allocated = alloc_resources(&res, *tmpjob)) == 0 && tmpQueue->next->next != NULL){
                    run_process(tmpQueue, priority3, argv);
                }
                
                if(allocated == 0){
                    reset_resources(&res);
                    allocated = alloc_resources(&res, *tmpjob);
                }
                tmpjob->allocated = allocated;

                
                push(tmpQueue,*tmpjob);
                pop(priority3);


            }
            
            if(tmpQueue->next->next == NULL && priority3->next == NULL){
                run_process(tmpQueue, priority3, argv);
            }
        }else{
            if(tmpQueue->next->next != NULL){
                run_process(tmpQueue, priority3, argv);
            }else{
                push(priority3, pop(priority3));
                clear_mem(&res);
            }
        }
    }

    return EXIT_SUCCESS;
}

void print_process(process proc){
    printf("Arrival Time: %d Priority: %d Time Left: %d Size: %dbytes Printers: %d Scanners: %d Modems: %d CDs: %d\n", proc.arrivalTime, proc.priority, proc.processTime, proc.memBytes, proc.printers, proc.scanners, proc.modems, proc.cds);
}




void run_process(node_t * pullQueue, node_t * pushQueue, char *argv[]){
    int stat;
    pid_t pid;

    process *proc;
    process popjob;
    popjob = pop(pullQueue);
    proc = &popjob;
    
    pid = fork();
    proc->pid = (int) pid;

    
    if(pid < (pid_t) 0){
        stat = -1;
        exit(1);
    }

    if(pid == 0){ 

        execv("./process", argv);
    }else{ 
        print_process(*proc);

        
        if(proc->priority == 0){
            
            sleep(proc->processTime);
            kill(proc->pid, SIGINT);
            waitpid(proc->pid, &stat, 0);

            
            free_mem(&res, proc->memAddress, proc->memBytes);

        }else{
            
            if(proc->paused == 0){
                
                sleep(1);
                proc->processTime--;
                
                kill(proc->pid, SIGTSTP);
                proc->paused = 1;

            }else{
                
                kill(proc->pid, SIGCONT);
                
                sleep(1);
                proc->processTime--;
                
                kill(proc->pid, SIGTSTP);
                proc->paused = 1;

            }
            
            free_mem(&res, proc->memAddress, proc->memBytes);
            free_resources(&res, *proc);

            
            if(proc->priority == 1){
                proc->priority++;
            }else if(proc->priority ==2){
                proc->priority++;
            }else if(proc->priority == 3){
                proc->priority=3;
            }
            
            if(proc->processTime <= 0){
                sleep(1);
                proc->processTime--;
                kill(proc->pid, SIGINT);

            }else{
                push(pushQueue, *proc);
            }

        }

    }
}
