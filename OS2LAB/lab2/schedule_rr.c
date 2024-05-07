#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *tasks = NULL;
struct node* currNode = NULL;

Task *pickNextTask() {
    if(!currNode)
        currNode = tasks;
    if(!currNode)
        return NULL;

    Task* taskRun = currNode->task;
    currNode = currNode->next;

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
    currNode = tasks;
    Task *taskRun = pickNextTask();
    while (taskRun) {
        run(taskRun, QUANTUM);
        taskRun->burst -= QUANTUM;
        if (taskRun->burst <= 0) {
            delete(&tasks, taskRun);
        }
        taskRun = pickNextTask();
    }
}



void schedule() {
    schedule_helper();
}