#ifndef IO_UTILITY_H
#define IO_UTILITY_H

class ioutility
{
public:
	ioutility();
	~ioutility();

	static int ioread(int fd, char* buf, int size);
	static int iowrite(int fd, char*buf, int size);
};

#endif