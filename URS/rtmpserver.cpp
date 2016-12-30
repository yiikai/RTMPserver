#include "rtmpserver.h"
#include "tcplistener.h"
rtmpserver::rtmpserver():m_cp(NULL)
{

}

rtmpserver::~rtmpserver()
{

}

void rtmpserver::init()
{
	m_cp = new cycleepoll();
}

void rtmpserver::rtmp_listen()
{
	connection* conn = new tcplistener();
	conn->createconnection(NULL,1935);
	m_cp->addlistenconnection(conn);

	//start cycle
	m_cp->startcycle();
}
