#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#include "listener.h"

class tcplistener : public listener
{
public:
	tcplistener();
	~tcplistener();
	bool createlistener(char* ip, int port);
	void dostreamingservice();
	connection* acceptclient();
private:

};

#endif
