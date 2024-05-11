/**
 * list data structure containing the tasks in the system
 */

typedef struct
{
    void (*function)(void *p);
    void *data;
}
        task;

struct node {
    task *task;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, task *task);
void delete(struct node **head, task *task);
void traverse(struct node *head);
