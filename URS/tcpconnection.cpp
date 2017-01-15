#include "tcpconnection.h"
#include "rtmpcomplexhandshake.h"
#include <stdio.h>
tcpconnection::tcpconnection(int fd) :connection(), m_status(RTMPSTATUS_HANDSHAKEC0C1)
{
	m_sockfd = fd;
	m_protocol = new rtmpprotocol(m_sockfd);
}

tcpconnection::~tcpconnection()
{

}

bool tcpconnection::createconnection(char* ip, int port)
{
	return true;
}

void tcpconnection::pullandsendstream()
{
	printf("client use protocol pull stream\n");
	m_protocol->pull_and_send_stream();
}


void tcpconnection::dostreamingservice()
{
		switch(m_status)
		{
		case RTMPSTATUS_NOCONNECT:
			case RTMPSTATUS_HANDSHAKEC0C1:
			{
				m_protocol->handle_handshakeC0C1();      
				m_status = RTMPSTATUS_HANDSHAKEC2;
			}break;
			case RTMPSTATUS_HANDSHAKEC2:
			{
				m_protocol->handle_handshakeC2();
				m_status = RTMPSTATUS_RTMPCONNECT;
			}break;
			case RTMPSTATUS_RTMPCONNECT:
			{
				m_protocol->connectapp();
				m_status = RTMPSTATUS_STREAMSERVICE;	
			}break;
			case RTMPSTATUS_STREAMSERVICE:
			{
				m_protocol->stream_service_cycle();
			}break;
			default:
			{
				printf("not right status\n");
			}break;
		}
		
}
