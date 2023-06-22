#ifndef _SEM_H
#define _SEM_H

#include<linux/sched.h>
#define TABLE_LEN 	30
#define NAME_LEN	15

struct taskQueue{
	struct task_struct *task;
    struct taskQueue* next;
};

struct sem_t{
	int value;
	char name[NAME_LEN];
	struct taskQueue* queue;
};

#endif