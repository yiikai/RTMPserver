#include "ioutility.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
ioutility::ioutility()
{

}

ioutility::~ioutility()
{

}

int ioutility::ioread(int fd, char* buf, int size)
{
	int allnum = size;
	int ret = read(fd, buf, size);
	return ret;
}


/*
when iowrite ret value is -1, which means the client has stop connect
Server should reset the client and clean from epoll tree
*/
int ioutility::iowrite(int fd, char*buf, int size)
{
	int allnum = size;
	int ret = write(fd, buf, size);
	if (ret == -1)
	{
		if (errno == 32)
		{
			//it can according errno = 32 to check whether client has closed
			printf("write return -1 errorno is %s\n", strerror(errno));
			return CLIENT_CLOSED;
		}
		else
		{
			return IO_WRITE_ERROR;
		}
	}
	return ret;
}
