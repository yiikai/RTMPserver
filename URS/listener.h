#ifndef LISTENER_H
#define LISTENER_H
#include "connection.h"

class listener : public connection
{
public:
	listener();
	virtual ~listener();
	bool createconnection(char* ip, int port);
	void dostreamingservice(){}
	void pullandsendstream(){}
protected:
	virtual bool createlistener(char* ip, int port) = 0;
};

#endif