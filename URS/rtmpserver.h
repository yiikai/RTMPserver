#ifndef RTMP_SERVER_H
#define RTMP_SERVER_H
#include "cycleepoll.h"
class rtmpserver
{
public:
	rtmpserver();
	~rtmpserver();
	void init();
	void rtmp_listen();
private:
	cycleepoll *m_cp;
	
};


#endif