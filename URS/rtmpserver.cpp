#include "rtmpserver.h"
#include "tcplistener.h"
#include <assert.h>
rtmpserver* rtmpserver::m_instance = NULL;

rtmpserver::rtmpserver():m_serverstarted(false),m_cp(NULL)
{
    init();
}

rtmpserver::~rtmpserver()
{

}

void rtmpserver::init()
{
	m_cp = new cycleepoll();
    m_listenconn = new tcplistener();
	m_listenconn->createconnection(NULL,1935);
}

void rtmpserver::rtmp_listen()
{
    assert(!m_serverstarted);
	if(!m_serverstarted)
    {
	    m_cp->addlistenconnection(m_listenconn);
	    //start cycle	 
        m_cp->startcycle();
        m_serverstarted = true;
    }
}

rtmpserver* rtmpserver::getInstance()
{
    if(m_instance)
    {
        return m_instance;
    }
    else
    {
        m_instance = new rtmpserver();
    }
    return m_instance;
}