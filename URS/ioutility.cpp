#include "ioutility.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

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
		if (errno == ETIMEDOUT)
		{
			//printf("write data to socket error, %s\n", strerror(errno));
			return -1;
		}
		else if (errno == ECONNRESET)
		{
			//printf("write data to socket error, %s\n", strerror(errno));
			return -1;
		}
		else if (errno == EPIPE)
		{
			//printf("write data to socket error, %s\n", strerror(errno));
			return -1;
		}
		else
		{
			//printf("write data to socket error, %s\n", strerror(errno));
			return -1;
		}
	}
	return ret;
}
