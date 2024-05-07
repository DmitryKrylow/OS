#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"


struct node *tasks = NULL;
struct node *queue = NULL;



Task *pickNextTask() {
    if(!tasks)
        return NULL;
    if(!queue)
        return NULL;

    Task* taskRun = queue->task;
    queue = queue->next;

    return taskRun;
}

Task *pickNextTaskQueue() {
    struct node* currNode = tasks->next;
    Task* taskRun = tasks->task;

    while(currNode){
        Task* nextTask = currNode->task;
        if(nextTask->priority == taskRun->priority)
            taskRun = nextTask;
        currNode = currNode->next;
    }

    return taskRun;
}


void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&tasks, newTask);
}

void schedule_helper_rr() {
    Task* taskRun = pickNextTask();
    while(taskRun){
        run(taskRun,QUANTUM);
        taskRun->burst-=QUANTUM;
        if(taskRun->burst <= 0){
            delete(&tasks,taskRun);
        }
        taskRun = pickNextTask();
    }
}


void schedule_helper() {
    while(tasks){
        struct node* currNode = tasks;
        Task *currTask = currNode->task;

        insert(&queue,currTask);
        currNode = currNode->next;

        while(currNode){
            currTask = currNode->task;
            currNode = currNode->next;
            if(currTask->priority > queue->task->priority){
                queue = NULL;
                insert(&queue, currTask);
            }else if(currTask->priority == queue->task->priority){
                insert(&queue, currTask);
            }
        }
        schedule_helper_rr();
    }
}


void schedule() {
    schedule_helper();
}