#ifndef CONNECTION_H
#define CONNECTION_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

class connection
{
public:
	connection();
	virtual ~connection();

	int getfd(){
		return m_sockfd;
	}

	virtual bool createconnection(char* ip, int port) = 0;
	virtual void dostreamingservice() = 0;
	virtual void pullandsendstream() = 0; 
protected:
	int m_sockfd;
};

#endif