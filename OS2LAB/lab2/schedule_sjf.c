#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *tasks = NULL;


Task *pickNextTask() {
    struct node* currNode = tasks->next;
    Task* taskRun = tasks->task;
    while(currNode != NULL){
        Task* nextTask = currNode->task;
        if(!nextTask)
            break;
        if(nextTask->priority > taskRun->priority)
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

void schedule_helper() {
    while(tasks){
        Task* taskRun = pickNextTask();
        run(taskRun,taskRun->burst);
        delete(&tasks,taskRun);
    }
}

void schedule() {
    schedule_helper();
}