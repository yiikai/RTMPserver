#ifndef RTMP_SERVER_H
#define RTMP_SERVER_H
#include "cycleepoll.h"
/*
class:rtmpserver
Singleton. Make sure not use for more than one creator
*/

class connection;
class rtmpserver
{
public:
    static rtmpserver* getInstance();
	~rtmpserver();
	void rtmp_listen();
private:
    rtmpserver();
    rtmpserver(const rtmpserver& server){}
    rtmpserver& operator=(const rtmpserver& server){return *this;}
    void init();
private:
    bool m_serverstarted;
	cycleepoll *m_cp;
    connection* m_listenconn;
    static rtmpserver* m_instance;
	
};


#endif