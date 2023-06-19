#include <errno.h>
#include <asm/segment.h>

#include <string.h>

#define MAX_SIZE 23

char buffer[MAX_SIZE+5];

int sys_iam(const char *name)
{
    char tmp[MAX_SIZE+6];
    int i = 0;
    // 从用户空间中读取数据
    while(i <= MAX_SIZE+5)
    {
        tmp[i] = get_fs_byte(name+i);
        if(tmp[i] == '\0') break;
        i++;
    }
    if(i<=MAX_SIZE){
        strcpy(buffer, tmp);
    }else{
        return -EINVAL;
    }
    return i;
}

int sys_whoami(char *name, unsigned int size)
{
    int len = strlen(buffer);
    if (size < len)
    {
        return -EINVAL;
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            put_fs_byte(buffer[i], name + i);
        }
    }
    return len;
}
