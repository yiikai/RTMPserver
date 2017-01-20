#ifndef FLV_CODEC_H
#define FLC_CODEC_H

/*
 *singlotn, Use for flv demux and decode
 */
#define FLV_VIDEO_KEY_FRAME		0x01
#define FLV_VIDEO_CODEC_AVC 	0x07
#define FLV_VIDEO_SEQUENCE_HEAD 0x00

class flvcodec
{
	public:
		~flvcodec();
		static flvcodec* getInstance();
		bool video_is_sequence_header(const char* playload, const int size);
	private:
		flvcodec();
	private:
		static flvcodec *m_instance;

};


#endif
