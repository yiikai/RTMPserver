#include "tsmuxer.h"
#include <stdio.h>

tsmuxer::tsmuxer() :m_avccodec(NULL)
{
	m_avccodec = new avccodec();
}

tsmuxer::~tsmuxer()
{

}

tsmuxer::tsmuxer(const tsmuxer& muxer)
{

}

tsmuxer& tsmuxer::operator = (const tsmuxer& muxer)
{

}

int tsmuxer::on_video(const char* data, const int length)
{
	if (m_avccodec->avcdemux(data, length) == -1)
		return -1;
}