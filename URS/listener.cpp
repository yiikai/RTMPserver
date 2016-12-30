#include "listener.h"

listener::listener():connection()
{

}


listener::~listener()
{

}


bool listener::createconnection(char* ip, int port)
{
	return createlistener(ip,port);
}
