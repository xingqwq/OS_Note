#define __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define NUMBER 550
#define CONSUMER 5
#define BUFSIZE 10

_syscall2(struct sem_t*, sem_open, const char *, name, unsigned int, value);
_syscall1(int, sem_wait, struct sem_t*, sem);
_syscall1(int, sem_post, struct sem_t*, sem);
_syscall1(int, sem_unlink, const char *, name);

int main()
{
    struct sem_t *empty, *full, *mutex;
    int fin, fout;
    int i, j, readData, pid;
    int cnt = 0;
    pid_t p;
    if ((empty = sem_open("empty_t", BUFSIZE)) == NULL)
    {
        return -1;
    }
    if ((mutex = sem_open("mutex_t", 1)) == NULL)
    {
        return -1;
    }
    if ((full = sem_open("full_t", 0)) == NULL)
    {
        return -1;
    }
    fin = open("buffer.dat", O_CREAT | O_WRONLY | O_TRUNC, 0222);
    fout = open("buffer.dat", O_RDONLY | O_TRUNC, 0444);
    if ((p = fork()) == 0)
    {
        printf("Producer Created.\n");
        fflush(stdout);
        for (i = 0; i < NUMBER; i++)
        {
            sem_wait(empty);
            /* printf("empty in.\n");*/
            sem_wait(mutex);
            /* printf("mutex in.\n");*/
            if (i % BUFSIZE == 0)
            {
                lseek(fin, 0, 0);
            }
            write(fin, (char *)&i, sizeof(int));
            /*printf("Producer %d: %d\n", getpid(), i);*/
            /*fflush(stdout);*/
            sem_post(mutex);
            /*printf("mutex out.\n");*/
            sem_post(full);
        }
        sleep(2);
        return 0;
    }
    else if (p < 0)
    {
        perror("Fork Error.\n");
        return -1;
    }
    for (j = 0; j < CONSUMER; j++)
    {
        if ((p = fork()) == 0)
        {
            printf("Consumer Created.\n");
            while(1)
            {
                sem_wait(full);
                sem_wait(mutex);
                if (!read(fout, (char *)&readData, sizeof(int)))
                {
                    lseek(fout, 0, 0);
                    read(fout, (char *)&readData, sizeof(int));
                }
                printf("%d: %d\n", getpid(), readData);
                fflush(stdout);
                sem_post(mutex);
                sem_post(empty);
            }
        }
        else if (p < 0)
        {
            perror("Fail to fork!\n");
            return -1;
        }
    }
    wait(NULL);
    sem_unlink("empty_t");
    sem_unlink("mutex_t");
    sem_unlink("full_t");
    close(fin);
    close(fout);
    return 0;
}