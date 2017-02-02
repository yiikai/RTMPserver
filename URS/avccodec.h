#ifndef AVC_CODEC_H
#define AVC_CODEC_H
/*
1 = key frame(for AVC, a seekable frame)
2 = inter frame(for AVC, a non - seekable frame)
3 = disposable inter frame(H.263 only)
4 = generated key frame(reserved for server use only)
5 = video info / command frame
*/
#define FRAME_TYPE_KEY_FRAME 1
#define FRAME_TYPE_inter_FRAME 2
#define FRAME_TYPE_disposable_inter_FRAME 3
#define FRAME_TYPE_generated_key_FRAME 4
#define FRAME_TYPE_inf_command_FRAME 5

/*
2 = Sorenson H.263
3 = Screen video
4 = On2 VP6
5 = On2 VP6 with alpha channel
6 = Screen video version 2
7 = AVC
*/

#define CODEC_ID_AVC 7

class avccodec
{
public:
	avccodec();
	~avccodec();

	avccodec(const avccodec& codec);
	avccodec& operator=(const avccodec& codec);

	int avcdemux(const char* data, const int length);
private:
	int avc_decoder_configuration_record_demux(const char* data, const int length);
private:
	char* sequenceParameterSetNALUnit;
	char* pictureParameterSetNALUnit;
};

#endif