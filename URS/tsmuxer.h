#ifndef TS_MUXER_H
#define TS_MUXER_H
#include "avccodec.h"
#include <stdio.h>
//use ffmpeg
#if defined  __cplusplus
	extern "C"
	{
		#include <libavutil/opt.h>
		#include <libavformat/avformat.h>
	}
#endif


enum TsStream
{
	TsStreamAudioAAC        = 0x0f,
	TsStreamVideo264        = 0x1b,
};

#define TS_PMT_NUMBER 1
#define TS_PMT_PID 0x1001
#define TS_VIDEO_AVC_PID 0x100
#define TS_AUDIO_AAC_PID 0x101

#if 0
class tspacket{
	public:
		tspacket();
		~tspacket();

	public:
		char sync_byte;   //0x47
		char transport_error_indicator:1;
		char payload_unit_start_indicator:1;
		charansport_priority:1;
		short pid:13;
		char adaption_field_control:2;
		char continuity_counter:4;
};
#endif
class rtmpstreamsource;
typedef int(*callback)(void* opaque,uint8_t* buf, int buf_size);
class tsmuxer
{
public:
	tsmuxer();
	~tsmuxer();

	tsmuxer(const tsmuxer& muxer);
	tsmuxer& operator=(const tsmuxer& muxer);

	int on_video();
	int close_muxer();

	int open_muxfile(callback func,rtmpstreamsource* source);
	int init_muxfile();
private:
	int write_segment(avcsample& sample, int dts);
	int insert_avc_frame(sampleST& sample);
	int cache_video(avcsample& sample);
	int flush_video_in_segment();
	int encode_pat_pmt();
	static int read_buffer(void* opaque, uint8_t* buf, int buf_size);
private:
	avccodec* m_avccodec;
	vector<char> m_muxvideo;  // The data need be muxed in ts file	
	bool m_firstframe;
	bool m_haspmtpat;
	int m_startpts;
	FILE* m_file;
	
	//ffmpeg
	AVOutputFormat *m_fmt;
	AVFormatContext *m_ic;
	AVFormatContext *m_oc;
	AVIOContext *m_avio;
	AVStream *audio_st;
	AVStream *video_st;
	AVCodec *audio_codec;
	AVCodec *video_codec;
	int m_videoindex;
	int out_videoindex;	
	int frame_index;
	long pts_time;
	vector<char> spspps;
};

#endif
