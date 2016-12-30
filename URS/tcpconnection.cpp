#include "tcpconnection.h"
#include "rtmpcomplexhandshake.h"
#include <stdio.h>
tcpconnection::tcpconnection(int fd):connection(),m_status(handshakeC0C1)
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
			case noconnect:
			case handshakeC0C1:
			{
                //接收到C0C1之后，首先要对于两种握手的形式进行判断
                m_protocol->readC0C1();
                c1s1 c1;
                Hschem sch = HAND_SHAKE_SCHEMA0;
                c1.parse(m_protocol->getC0C1()+1,1536,sch);
                bool is_valid = false;
                int ret;
                HandType type = complex;
                m_protocol->setHandShakeMethod(type);
                if ((ret = c1.c1_validate_digest(is_valid)) != 0 || !is_valid) 
                {
                    printf("schema0 failed, try schema1.\n");
                    sch = HAND_SHAKE_SCHEMA1;
                    c1.parse(m_protocol->getC0C1()+1,1536,sch);
                    if ((ret = c1.c1_validate_digest(is_valid)) != 0 || !is_valid) {
                        //ret = ERROR_RTMP_TRY_SIMPLE_HS;
                        HandType type = simple;
                        m_protocol->setHandShakeMethod(type);
                        printf("all schema valid failed, try simple handshake. ret=%d", ret);
                    }
                    HandType type = complex;
                    m_protocol->setHandShakeMethod(type);
                }

                //根据握手类型设定之后的交互方式
                
				//m_protocol->readC0C1();
				m_protocol->createS0S1S2(c1);
				m_status = handshakeC2;
			}break;
			case handshakeC2:
			{
                int ret;
				m_protocol->readC2();
                c2s2 c2;
                c2.parse(m_protocol->getC2(), 1536);
             
                printf("complex handshake read c2 success.\n");
                
                // verify c2
                // never verify c2, for ffmpeg will failed.
                // it's ok for flash.
                printf("complex handshake success\n");
				m_status = rtmpconnect;
			}break;
			case rtmpconnect:
			{
				m_protocol->connectapp();
				m_status = streamservice;	
			}break;
			case streamservice:
			{
				m_protocol->stream_service_cycle();
			}break;
			default:
			{
				printf("not right status\n");
			}break;
		}
		
}
