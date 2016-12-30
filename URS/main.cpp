#include "rtmpserver.h"

int main()
{
	rtmpserver rs;
	rs.init();
	rs.rtmp_listen();
	return 0;
}