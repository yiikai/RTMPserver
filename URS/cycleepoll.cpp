#include "cycleepoll.h"
#include "tcplistener.h"
#include <errno.h>

int cycleepoll::m_epollfd = -1;
map<int, connection*> cycleepoll::m_fdmap;
map<int, connection*> cycleepoll::m_fdclimap;

cycleepoll::cycleepoll():m_stop(false)
{
	init();
}

cycleepoll::~cycleepoll()
{
    if(!m_fdmap.empty())
    {
        map<int,connection*>::iterator itr = m_fdmap.begin();
        for(;itr != m_fdmap.end(); itr++)
        {
            delete (*itr).second;
        }
        m_fdmap.clear();
    }

    if(!m_fdclimap.empty())
    {
        map<int,connection*>::iterator itr = m_fdclimap.begin();
        for(;itr != m_fdclimap.end(); itr++)
        {
            delete (*itr).second;
        }
        m_fdclimap.clear();
    }

    close(m_epollfd);
   
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
		//printf("epool mod fd error, %s\n",strerror(errno));
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
    if(m_fdmap.find(conn->getfd())->second)
    {
        m_fdmap.erase(conn->getfd());
        epoll_ctl(m_epollfd,EPOLL_CTL_DEL,conn->getfd(),NULL);
    }
}
	
void cycleepoll::addclientconnection(connection* conn)
{
	m_fdclimap.insert(pair<int,connection*>(conn->getfd(),conn));
	struct epoll_event ep_event;
    ep_event.events = EPOLLIN;
    ep_event.data.fd = conn->getfd();
    int ret = epoll_ctl(m_epollfd,EPOLL_CTL_ADD,conn->getfd(),&ep_event);
	printf("add epoll fd %d\n",ret);
}

void cycleepoll::delclientconnection(connection* conn)
{
    if(m_fdclimap.find(conn->getfd())->second)
    {
        m_fdclimap.erase(conn->getfd());
        epoll_ctl(m_epollfd,EPOLL_CTL_DEL,conn->getfd(),NULL);
    }
}

void cycleepoll::delclientconnection(int fd)
{
	if (m_fdclimap.find(fd)->second)
	{
		m_fdclimap.erase(fd);
		epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
	}
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
			//printf("no event is ready\n");
			break;
		}
		else if(fdnum == 0)
		{
			continue;
		}
		//printf("active fd num is %d\n",fdnum);
		for(int i = 0; i < fdnum; i++)
		{
			//printf("fd number is %d\n",events[i].data.fd);

			if(events[i].data.fd == 11)
			{
				printf("client arrive\n");
			}

			map<int,connection*>::iterator itr;
			itr = m_fdmap.find(events[i].data.fd);
			if(itr != m_fdmap.end())  //check if current active fd is Server listen fd
			{
				//printf("active client fd is %d\n",events[i].data.fd);
				connection* cli = ((tcplistener*)(itr->second))->acceptclient();
				addclientconnection(cli);
				continue;
			}
			itr = m_fdclimap.find(events[i].data.fd);
			if(itr == m_fdclimap.end())
			{
				//printf("This connection is not exist in client map , cli fd is %d\n",events[i].data.fd);
				continue;
			}
			if(events[i].events == EPOLLOUT)
			{
				//If events can write it must be a client connect to server , so need pull AV stream 
				//printf("client %d want pull stream and need send AV stream\n",events[i].data.fd);
				int value = itr->second->pullandsendstream();
				printf("value is %d\n", value);
				if ( -1 == value )
				{
					//printf("client has stoped connect with server, remove the client from server\n");
					delclientconnection(events[i].data.fd);
				}
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
