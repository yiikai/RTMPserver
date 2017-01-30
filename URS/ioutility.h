#ifndef IO_UTILITY_H
#define IO_UTILITY_H

#define IO_WRITE_ERROR -1
#define CLIENT_CLOSED -2

class ioutility
{
public:
	ioutility();
	~ioutility();

	static int ioread(int fd, char* buf, int size);
	static int iowrite(int fd, char*buf, int size);
};

#endif