#define __LIBRARY__
#include <errno.h>
#include <unistd.h>

_syscall2(int, whoami,char*,name,unsigned int,size);

int main(int argc, char ** argv)
{
	char tmp[30];
	whoami(tmp, 24);
	printf("%s\n", tmp);
	return 0;
}