#include <stdio.h>
#include "flvcodec.h"

flvcodec* flvcodec::m_instance = NULL;

flvcodec::flvcodec()
{

}

flvcodec::~flvcodec()
{

}

flvcodec* flvcodec::getInstance()
{
	if(!m_instance)
	{
		m_instance = new flvcodec();
		if(!m_instance)
			return NULL;
		else
			return m_instance;
	}
	return m_instance;
}

bool flvcodec::video_is_sequence_header(const char* playload, const int size)
{
	char codec_id = playload[0] & 0x0f;
	if(codec_id != FLV_VIDEO_CODEC_AVC)
	{
		printf("This message is not video data \n");
		return false;
	}
	if(playload[1] != FLV_VIDEO_SEQUENCE_HEAD)
	{
		printf("This message playload is not video sequence head\n");
		return false;
	}
	return true;
}

