#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "connection.h"
#include "rtmpprotocol.h"



class tcpconnection : public connection{

public:
	tcpconnection(int fd);
	~tcpconnection();

	bool createconnection(char* ip, int port);
	void dostreamingservice();

	int pullandsendstream();

private:
	RTMPSTATUS m_status;
	rtmpprotocol *m_protocol;
};

#endif
