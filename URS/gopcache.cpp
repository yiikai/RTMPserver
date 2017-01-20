#include "gopcache.h"
#include <stdio.h>

gopcache::gopcache()
{
	
}
	
gopcache::~gopcache()
{

}

void gopcache::add_gop_cache(const sharedMessage& msg)
{
	if(is_Ifream(msg))
	{
		m_cache.clear();
		m_cache.push_back(msg);
	}
	else
	{
		m_cache.push_back(msg);
	}
}

bool gopcache::is_Ifream(const sharedMessage& msg)
{
	char frame_type = (msg.messagepalyload[0]>>4) & 0x0f;
	char codecid = (msg.messagepalyload[0] & 0x0f);
	if(codecid != 7)  //不是AVC
	{
		 printf("Not a video frame\n");
		 return false;
	}
	if(frame_type != 1)
	{
		printf("not a video key frame\n");
		return false;
	}
	return true;
}
