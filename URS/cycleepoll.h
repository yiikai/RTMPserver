#ifndef CYCLE_EPOLL_H
#define CYCLE_EPOLL_H

#include <fcntl.h>
#include "connection.h"
#include <map>
#include <stdio.h>
using namespace std;

#define MAX_CONNECTION_SIZE 1024

enum fdtype
{
	readfd = 0,
	writefd = 1,
};

class cycleepoll
{
public:
	cycleepoll();
	~cycleepoll();

	void addlistenconnection(connection* conn);
	void dellistenconnection(connection* conn);

	void addclientconnection(connection* conn);
	void delclientconnection(connection* conn);
	static void delclientconnection(int fd);

	bool init();
	void startcycle();
	void stopcycle();

	static void modifyconnectionsocketfd(fdtype type, int fd);
private:
	static int m_epollfd;
	int m_stop;
	static map<int,connection*> m_fdmap;  //epoll 监听的LISTEN描述符
	static map<int,connection*> m_fdclimap;
};

#endif