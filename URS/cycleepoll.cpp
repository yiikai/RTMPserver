#include "cycleepoll.h"
#include "tcplistener.h"
#include <errno.h>

int cycleepoll::m_epollfd = -1;

cycleepoll::cycleepoll():m_stop(false)
{
	init();
}

cycleepoll::~cycleepoll()
{

}

void cycleepoll::modifyconnectionsocketfd(fdtype type,int fd)
{
	struct epoll_event ep_event;
	if(type == readfd)
	{
		ep_event.events = EPOLLIN;
	}else if(type == writefd)
	{
    	ep_event.events = EPOLLOUT;
	}
	ep_event.data.fd = fd;
    int ret = epoll_ctl(m_epollfd,EPOLL_CTL_MOD,fd,&ep_event);
	if(ret == -1)
	{
		printf("epool mod fd error, %s\n",strerror(errno));
	}
		
}


void cycleepoll::addlistenconnection(connection* conn)
{
	m_fdmap.insert(pair<int,connection*>(conn->getfd(),conn));
	struct epoll_event ep_event;
    ep_event.events = EPOLLIN;
    ep_event.data.fd = conn->getfd();
    epoll_ctl(m_epollfd,EPOLL_CTL_ADD,conn->getfd(),&ep_event);
}
	
void cycleepoll::dellistenconnection(connection* conn)
{

}
	
void cycleepoll::addclientconnection(connection* conn)
{
	m_fdclimap.insert(pair<int,connection*>(conn->getfd(),conn));
	struct epoll_event ep_event;
    ep_event.events = EPOLLIN;
    ep_event.data.fd = conn->getfd();
    epoll_ctl(m_epollfd,EPOLL_CTL_ADD,conn->getfd(),&ep_event);
}

void cycleepoll::delclientconnection(connection* conn)
{

}

bool cycleepoll::init()
{
	m_epollfd = epoll_create(MAX_CONNECTION_SIZE);
	if(m_epollfd == -1)
		return false;
	if (fcntl(m_epollfd, F_SETFL, O_NONBLOCK) == -1)
  	{
   		return false;
  	}
  	return true;
}


void cycleepoll::startcycle()
{
	while(!m_stop)
	{
		struct epoll_event events[MAX_CONNECTION_SIZE];
		int fdnum = epoll_wait(m_epollfd, events, MAX_CONNECTION_SIZE , -1);
		if(fdnum == -1)
		{
			printf("no event is ready\n");
			break;
		}
		else if(fdnum == 0)
		{
			continue;
		}
		printf("active fd num is %d\n",fdnum);
		for(int i = 0; i < fdnum; i++)
		{
			printf("number %d\n",i);
			map<int,connection*>::iterator itr;
			itr = m_fdmap.find(events[i].data.fd);
			if(itr != m_fdmap.end())
			{
				printf("active client fd is %d\n",events[i].data.fd);
				connection* cli = ((tcplistener*)(itr->second))->acceptclient();
				addclientconnection(cli);
				continue;
			}
			itr = m_fdclimap.find(events[i].data.fd);
			if(itr == m_fdclimap.end())
			{
				printf("This connection is not exist in client map , cli fd is %d\n",events[i].data.fd);
				continue;
			}
			if(events[i].events == EPOLLOUT)
			{
				//可写的socket一定是客户端进行播放的socket，需要拉去AV数据
				printf("client %d want pull stream and need send AV stream\n",events[i].data.fd);
				itr->second->pullandsendstream();
				continue;
			}else
			{
				itr->second->dostreamingservice();
			}
		}
	}
}

void cycleepoll::stopcycle()
{

}
