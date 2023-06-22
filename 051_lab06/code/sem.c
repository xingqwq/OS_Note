#include <unistd.h>
#include <linux/sem.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <linux/fdreg.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/segment.h>

struct sem_t *sem_table[TABLE_LEN];

int get_len(char *name, char* new)
{
    char tmp[NAME_LEN + 4];
    char c;
    int i;
    for (i = 0; i < NAME_LEN + 4; i++)
    {
        c = get_fs_byte(name + i);
        tmp[i] = c;
        if (c == '\0')
            break;
    }
    if (i >= NAME_LEN)
    {
        return -1;
    }
    else
    {
        strcpy(new, tmp);
        return i;
    }
}

int find_sem(char *name)
{
    int i;
    for (i = 0; i < TABLE_LEN; i++)
    {
        if (sem_table[i] == NULL)
        {
            continue;
        }
        if (strcmp(name, sem_table[i]->name) == 0)
        {
            return i;
        }
    }
    return -1;
}

void push_task(struct sem_t *sem, struct task_struct *p)
{
    if (sem->queue == NULL)
    {
        struct taskQueue *new = (struct taskQueue *)malloc(sizeof(struct taskQueue));
        new->task = p;
        sem->queue = new;
        return;
    }
    struct taskQueue *tmp = sem->queue;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    struct taskQueue *new = (struct taskQueue *)malloc(sizeof(struct taskQueue));
    new->task = p;
    tmp->next = new;
    return;
}

struct task_struct *pop_task(struct sem_t *sem)
{
    if (sem->queue == NULL)
    {
        return NULL;
    }
    struct task_struct *p = sem->queue->task;
    struct taskQueue *tmp = sem->queue;
    sem->queue = tmp->next;
    free(tmp);
    return p;
}

void sem_wait(struct sem_t *sem)
{
    cli();
    if (--sem->value<0)
    {
        push_task(sem, current);
        sleep_on(&current);
    }
    sti();
}

void sem_post(struct sem_t *sem)
{
    cli();
    struct task_struct *p;
    if (++sem->value<=0)
    {
        p = pop_task(sem);
        if (p != NULL)
        {
            wake_up(&p);
        }
    }
    sti();
}

struct sem_t *sem_open(const char *name, unsigned int value)
{
    char tmp_name[NAME_LEN + 5];
    int char_len = get_len(name, &tmp_name[0]);
    if (char_len < 0)
    {
        printk("Name Error.\n");
        return NULL;
    }
    printk("name: %s\n",tmp_name);
    int ret = find_sem(tmp_name);
    if (ret != -1)
    {
        return sem_table[ret];
    }
    else
    {
        int i = 0;
        for (i = 0; i < TABLE_LEN; i++)
        {
            if (sem_table[i] == NULL)
            {
                struct sem_t *new = (struct sem_t *)malloc(sizeof(struct sem_t));
                strcpy(new->name, tmp_name);
                new->value = value;
                sem_table[i] = new;
                return new;
            }
        }
        printk("Resource Error.\n");
        return NULL;
    }
}

int sem_unlink(const char *name)
{
    char tmp_name[NAME_LEN + 5];
    int char_len = get_len(name, &tmp_name[0]);
    if (char_len < 0)
    {
        printk("Name Error.\n");
        return NULL;
    }
    int ret = find_sem(tmp_name);
    if (ret != -1)
    {
        struct sem_t *tmp = sem_table[ret];
        free(tmp);
        sem_table[ret] = NULL;
        return 0;
    }
    return -1;
}