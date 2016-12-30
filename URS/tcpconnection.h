#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "connection.h"
#include "rtmpprotocol.h"
typedef enum RTMPSTATUS
{
	noconnect = -1,
	handshakeC0C1 = 0,
	handshakeC2,
	rtmpconnect,
	streamservice,
};

class tcpconnection : public connection{

public:
	tcpconnection(int fd);
	~tcpconnection();

	bool createconnection(char* ip, int port);
	void dostreamingservice();

	void pullandsendstream();

private:
	RTMPSTATUS m_status;
	rtmpprotocol *m_protocol;
};

#endif
