#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *tasks = NULL;

Task *pickNextTask() {
    if (!tasks)
        return NULL;

    Task *taskRun = tasks->task;
    delete(&tasks, taskRun);

    return taskRun;
}

void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&tasks, newTask);
}

void schedule_helper(Task *currTask) {
    if (!currTask){
        return;
    }

    Task *newTask = pickNextTask();
    schedule_helper(newTask);
    run(currTask,currTask->burst);
}

void schedule() {
    Task *currTask = pickNextTask();
    schedule_helper(currTask);
}