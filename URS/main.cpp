#include "rtmpserver.h"

int main()
{
    rtmpserver* server = rtmpserver::getInstance();
	server->rtmp_listen();
	return 0;
}