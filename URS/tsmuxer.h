#ifndef TS_MUXER_H
#define TS_MUXER_H
#include "avccodec.h"
class tsmuxer
{
public:
	tsmuxer();
	~tsmuxer();

	tsmuxer(const tsmuxer& muxer);
	tsmuxer& operator=(const tsmuxer& muxer);

	int on_video(const char* data, const int length);
private:
	avccodec* m_avccodec;
};

#endif