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
	//do
	//{
		int ret = read(fd,buf,size);
		//allnum -= ret;
	//}while(allnum);
	return ret;
}

int ioutility::iowrite(int fd, char*buf, int size)
{
	int allnum = size;
	//do
	//{
		int ret = write(fd,buf,size);
		if(ret == -1)
		{
			printf("write data to socket error, %s\n",strerror(errno));
		}
		//allnum -= ret;
	//}while(allnum);
	return ret;
}
