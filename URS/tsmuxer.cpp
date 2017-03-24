#include "tsmuxer.h"
#include "streamsource.h"
#include <stdio.h>

tsmuxer::tsmuxer() :m_avccodec(NULL),m_firstframe(true),m_haspmtpat(false),m_file(NULL)
					,m_fmt(NULL),m_ic(NULL),m_oc(NULL),m_avio(NULL),audio_st(NULL),video_st(NULL),frame_index(0)

{
	m_avccodec = new avccodec();
}

tsmuxer::~tsmuxer()
{

}

void* create_file_thread_func(void* arg)
{
	tsmuxer* mux = (tsmuxer*)arg;
	mux->init_muxfile();
	mux->on_video();
}


int tsmuxer::open_muxfile(callback func,rtmpstreamsource* source)
{
#if 0
	if(!m_file)
	{
		m_file = fopen("video.h264","ab");
	}
#endif
	av_register_all();
	avformat_alloc_output_context2(&m_oc,NULL,NULL,"vidoe.ts");
	if(!m_oc)
		return -1;
	unsigned char*aviobuffer = (unsigned char*)av_malloc(100*1024);
	m_avio = avio_alloc_context(aviobuffer,100*1024,0,source,func,NULL,NULL);
	m_ic = avformat_alloc_context();
	m_ic->pb = m_avio;
	
	return 0;
}

int tsmuxer::init_muxfile()
{
	int ret;
	ret = avformat_open_input(&m_ic,0,0,0);	
	if(ret != 0)
		return 1;  //return 1 for not open input m_ic  
	ret = avformat_find_stream_info(m_ic,0);
	if(ret != 0)
		return 2; //return 2 for not find stream info with m_ic
	ret = avformat_alloc_output_context2(&m_oc,0,0,"video.ts");
	if(ret != 0)
		return -1;

	for(unsigned int i = 0; i < m_ic->nb_streams; i++)
	{
		if(m_ic->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{	
			AVStream *in_stream = m_ic->streams[i];
			AVStream *out_stream = avformat_new_stream(m_oc,in_stream->codec->codec);
			if(!out_stream)
			{
				return -1;
			}
			if(avcodec_copy_context(out_stream->codec,in_stream->codec) < 0)
			{
				return -1;
			}
			m_videoindex = i;
			out_videoindex = out_stream->index;
		}	
	}


	if(!(m_fmt->flags & AVFMT_NOFILE))
	{
		int ret  = avio_open(&m_oc->pb,"video.ts",AVIO_FLAG_WRITE);
		if(ret < 0)
		{
			return -1;
		}
	}

	ret = avformat_write_header(m_oc,NULL);
	if(ret < 0)
		return -1;

	return 0;
}

int tsmuxer::close_muxer()
{
#if 0
	if(m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
#endif
	av_write_trailer(m_oc);
	avformat_close_input(&m_ic);  
	/* close output */  
	avio_close(m_oc->pb);  
	avformat_free_context(m_oc);  
	return 0;
}

tsmuxer::tsmuxer(const tsmuxer& muxer)
{

}

tsmuxer& tsmuxer::operator = (const tsmuxer& muxer)
{

}

int tsmuxer::on_video()
{
	AVPacket pkt;
	int stream_index=0;
	AVStream *in_stream =NULL, *out_stream = NULL; 
	if(av_read_frame(m_ic, &pkt) >= 0)
	{
		do{ 
			in_stream = m_ic->streams[pkt.stream_index];
			stream_index = out_videoindex;
			out_stream = m_oc->streams[stream_index];
			if(pkt.stream_index == m_videoindex)
			{
				if(pkt.pts==AV_NOPTS_VALUE)
				{
					AVRational time_base1=in_stream->time_base;
					int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
					pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
					pkt.dts=pkt.pts;
					pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
					frame_index++; 
				}
				break;

			}
		}while(av_read_frame(m_ic, &pkt) >= 0);
	}
	pkt.pts = av_rescale_q(pkt.pts, in_stream->time_base, out_stream->time_base);  
	pkt.dts = av_rescale_q(pkt.dts, in_stream->time_base, out_stream->time_base);  
	pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);  
	pkt.pos = -1;  
	pkt.stream_index=stream_index;  

	if (av_interleaved_write_frame(m_oc, &pkt) < 0) {  
		printf( "Error muxing packet\n");
	   	return -1;	
	}  
	av_free_packet(&pkt);	
}

int tsmuxer::flush_video_in_segment()
{
	if(!m_haspmtpat)
	{
		//write pmt pat table for each segment only once

	}
}

int tsmuxer::encode_pat_pmt()
{
	if(true)
	{

	}	
}

int tsmuxer::write_segment(avcsample& sample, int dts)
{
	//The CompositionTime (FLV spec p.72) tells the renderer when to perform (“compose”) the video frame on the display device in milliseconds after it enters the decoder; thus it is
//compositionTime = (PTS – DTS) / 90.0
	int pts = dts + sample.cts*90;
	if(m_firstframe)
	{
		m_startpts = pts;
	}	
	cache_video(sample);
	
	if((pts - m_startpts)/90000 < 10*1000)
	{
		//close segment
		return 0;
	}	

	return 0;
}

int tsmuxer::cache_video(avcsample& sample)
{
	//format to avc and use annexb then push in cache for write
	int ret = 0;
	vector<sampleST>::iterator itr = sample.m_nalusamples.begin();
	for( ;itr != sample.m_nalusamples.end(); itr++)
	{
		int nalu_type = (itr->data)[0] & 0x1f;
		if(nalu_type == SrsAvcNaluTypeIDR)
		{
#if 0
			//if get IDR frame need add sps and pps
			sampleST sps;
			sps.data = m_avccodec->sequenceParameterSetNALUnit;
			sps.size = m_avccodec->spslen;
			insert_avc_frame(sps);

			sampleST pps;
			pps.data = m_avccodec->pictureParameterSetNALUnit;
			pps.size = m_avccodec->ppslen;
			insert_avc_frame(pps);				
#endif
		}
		insert_avc_frame(*itr);	
	}	
	return ret;	
}

int getVopType( const void *p, int len )
{
	if ( !p || 6 >= len )
		return -1;
	unsigned char *b = (unsigned char*)p;
	// Verify NAL marker
	if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
	{   b++;
		if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
			return -1;
	} // end if
	b += 3;
	// Verify VOP id
	if ( 0xb6 == *b )
	{
		b++;
		return ( *b & 0xc0 ) >> 6;
	} // end if
	switch( *b )
	{
		case 0x65 : return 0;
		case 0x61 : return 1;
		case 0x01 : return 2;
	} // end switch
	return -1;
}

int tsmuxer::insert_avc_frame(sampleST& sample)
{
	static char fresh_nalu_header[] = { 0x00, 0x00, 0x00, 0x01 };
	static char cont_nalu_header[] = { 0x00, 0x00, 0x01 };

	if (m_firstframe) {
		m_firstframe = false;
		m_muxvideo.insert(m_muxvideo.end(),fresh_nalu_header, fresh_nalu_header+4);
	} else {
		m_muxvideo.insert(m_muxvideo.end(),cont_nalu_header, cont_nalu_header+3);
	}
	m_muxvideo.insert(m_muxvideo.end(),sample.data,sample.data+sample.size);

	//fwrite(m_muxvideo.data(),1,m_muxvideo.size(),m_file);
	//use ffmpeg
	AVPacket pkt = { 0 };
	av_init_packet(&pkt);
	AVRational time_base1={1,12000000};
	int64_t calc_duration=(double)AV_TIME_BASE/av_q2d((AVRational){50,2});
	pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
	pkt.dts=pkt.pts;
	pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
	frame_index++; 
	pkt.data = (unsigned char*)m_muxvideo.data();
	pkt.size = m_muxvideo.size();
	pkt.stream_index = video_st->index;


	pkt.pts = av_rescale_q(pkt.pts, video_st->time_base, video_st->time_base);  
	pkt.dts = av_rescale_q(pkt.dts, video_st->time_base, video_st->time_base);  
	pkt.duration = av_rescale_q(pkt.duration, video_st->time_base, video_st->time_base);  
	pkt.pos = -1;  


	int ret;
	ret = av_interleaved_write_frame(m_oc,&pkt);
	if(ret != 0)
		return -1;
	


	m_muxvideo.clear();	
	return 0;
}
	
