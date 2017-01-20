#include "tcplistener.h"
#include "tcpconnection.h"
#include <stdio.h>
#include <errno.h>
tcplistener::tcplistener()
{

}

tcplistener::~tcplistener()
{

}

bool tcplistener::createlistener(char* ip, int port)
{
	m_sockfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    servaddr.sin_port = htons(port);
    bind(m_sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    listen(m_sockfd,1500);
}

void tcplistener::dostreamingservice()
{
	
}

connection* tcplistener::acceptclient()
{
	//printf("new client has arrived need do someting\n");
	int clifd;
	struct sockaddr_in cliaddr;
	bzero(&cliaddr,sizeof(struct sockaddr_in));
    socklen_t  cliaddrlen = 1;
    clifd = accept(m_sockfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
     if (clifd == -1)
     {
         //printf("accpet error:");
		 //printf("%s\n",strerror(errno));
         return NULL;
     }
     else
     {
     	//添加一个客户描述符和事件
        //printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
        tcpconnection* clientconnect = new tcpconnection(clifd);
        return clientconnect;
     }
     return NULL;
}
