#include "tsmuxer.h"
#include <assert.h>
#include <string>
#include <string.h>

tspesplayload::tspesplayload(tspacket* pkt) :tsplayload(pkt)
{

}

tspesplayload::~tspesplayload()
{

}

int tspesplayload::size()
{
	int sz = 0;

	PES_header_data_length = 0;
	TsPESStreamId sid = (TsPESStreamId)stream_id;

	if (sid != TsPESStreamIdProgramStreamMap
		&& sid != TsPESStreamIdPaddingStream
		&& sid != TsPESStreamIdPrivateStream2
		&& sid != TsPESStreamIdEcmStream
		&& sid != TsPESStreamIdEmmStream
		&& sid != TsPESStreamIdProgramStreamDirectory
		&& sid != TsPESStreamIdDsmccStream
		&& sid != TsPESStreamIdH2221TypeE
		) {
		sz += 6;
		sz += 3;
		PES_header_data_length = sz;

		sz += (PTS_DTS_flags == 0x2) ? 5 : 0;
		sz += (PTS_DTS_flags == 0x3) ? 10 : 0;
		sz += ESCR_flag ? 6 : 0;
		sz += ES_rate_flag ? 3 : 0;
		sz += DSM_trick_mode_flag ? 1 : 0;
		sz += additional_copy_info_flag ? 1 : 0;
		sz += PES_CRC_flag ? 2 : 0;
		sz += PES_extension_flag ? 1 : 0;
		PES_header_data_length = sz - PES_header_data_length;
	}
	return sz;
}

int tspesplayload::encode(streamworker* stream)
{
	int ret = 0;

	// 6B fixed header.
	if (!stream->require(6)) {
		ret = -1;
		printf("ts: mux PSE failed.n");
		return ret;
	}

	// 3B
	stream->write3bytes(packet_start_code_prefix);
	// 1B
	stream->write1bytes(stream_id);
	// 2B
	// the PES_packet_length is the actual bytes size, the pplv write to ts
	// is the actual bytes plus the header size.
	int pplv = 0;
	if (PES_packet_length > 0) {
		pplv = PES_packet_length + 3 + PES_header_data_length;
		pplv = (pplv > 0xFFFF) ? 0 : pplv;
	}
	stream->write2bytes(pplv);

	// check the packet start prefix.
	packet_start_code_prefix &= 0xFFFFFF;
	if (packet_start_code_prefix != 0x01) {
		ret = -1;
		printf("ts: mux PSE start code failed, expect=0x01, actual=%#x. ret=%d", packet_start_code_prefix, ret);
		return ret;
	}

	// 3B flags.
	if (!stream->require(3)) {
		ret = -1;
		printf("ts: mux PSE flags failed. \n");
		return ret;
	}
	// 1B
	int oocv = original_or_copy & 0x01;
	oocv |= (const2bits << 6) & 0xC0;
	oocv |= (PES_scrambling_control << 4) & 0x30;
	oocv |= (PES_priority << 3) & 0x08;
	oocv |= (data_alignment_indicator << 2) & 0x04;
	oocv |= (copyright << 1) & 0x02;
	stream->write1bytes(oocv);
	// 1B
	int pefv = PES_extension_flag & 0x01;
	pefv |= (PTS_DTS_flags << 6) & 0xC0;
	pefv |= (ESCR_flag << 5) & 0x20;
	pefv |= (ES_rate_flag << 4) & 0x10;
	pefv |= (DSM_trick_mode_flag << 3) & 0x08;
	pefv |= (additional_copy_info_flag << 2) & 0x04;
	pefv |= (PES_CRC_flag << 1) & 0x02;
	stream->write1bytes(pefv);
	// 1B
	stream->write1bytes(PES_header_data_length);

	// check required together.
	int nb_required = 0;
	nb_required += (PTS_DTS_flags == 0x2) ? 5 : 0;
	nb_required += (PTS_DTS_flags == 0x3) ? 10 : 0;
	nb_required += ESCR_flag ? 6 : 0;
	nb_required += ES_rate_flag ? 3 : 0;
	nb_required += DSM_trick_mode_flag ? 1 : 0;
	nb_required += additional_copy_info_flag ? 1 : 0;
	nb_required += PES_CRC_flag ? 2 : 0;
	nb_required += PES_extension_flag ? 1 : 0;
	if (!stream->require(nb_required)) {
		ret = -1;
		printf("ts: mux PSE payload failed.\n");
		return ret;
	}
#if 0
	// 5B
	if (PTS_DTS_flags == 0x2) {
		if ((ret = encode_33bits_dts_pts(stream, 0x02, pts)) != ERROR_SUCCESS) {
			return ret;
		}
	}

	// 10B
	if (PTS_DTS_flags == 0x3) {
		if ((ret = encode_33bits_dts_pts(stream, 0x03, pts)) != ERROR_SUCCESS) {
			return ret;
		}
		if ((ret = encode_33bits_dts_pts(stream, 0x01, dts)) != ERROR_SUCCESS) {
			return ret;
		}

		// check sync, the diff of dts and pts should never greater than 1s.
		if (dts - pts > 90000 || pts - dts > 90000) {
			srs_warn("ts: sync dts=%"PRId64", pts=%"PRId64, dts, pts);
		}
	}
#endif

	return ret;
}


tsplayload::tsplayload(tspacket* pkt) :m_packet(NULL)
{
	m_packet = pkt;
}
tsplayload::~tsplayload()
{

}

tspsiplayload::tspsiplayload(tspacket* pkt) :tsplayload(pkt)
{
	pointer_field = 0;
	section_syntax_indicator = 1;
	const0_value = 0;
	reserved = 3;
	CRC_32 = 0;
}

tspsiplayload::~tspsiplayload()
{

}

int tspsiplayload::encode(streamworker* stream)
{
	int ret = 0;

	if (m_packet->payload_unit_start_indicator) {
		if (!stream->require(1)) {
			ret = -1;
			printf("ts: mux PSI failed. \n");
			return ret;
		}
		stream->write1bytes(pointer_field);
	}

	// to calc the crc32
	char* ppat = stream->data() + stream->pos();
	int pat_pos = stream->pos();

	// atleast 3B for all psi.
	if (!stream->require(3)) {
		ret = -1;
		printf("ts: mux PSI failed.\n");
		return ret;
	}
	// 1B
	stream->write1bytes(table_id);

	// 2B
	short slv = section_length & 0x0FFF;
	slv |= (section_syntax_indicator << 15) & 0x8000;
	slv |= (const0_value << 14) & 0x4000;
	slv |= (reserved << 12) & 0x3000;
	stream->write2bytes(slv);

	// no section, ignore.
	if (section_length == 0) {
		printf("ts: mux PAT ignore empty section");
		return ret;
	}

	if (!stream->require(section_length)) {
		ret = -1;
		printf("ts: mux PAT section failed. \n");
		return ret;
	}

	// call the virtual method of actual PSI.
	if ((ret = psi_encode(stream)) != 0) {
		return ret;
	}

	// 4B
	if (!stream->require(4)) {
		ret = -1;
		printf("ts: mux PSI crc32 failed. \n");
		return ret;
	}
	//CRC_32 = srs_crc32(ppat, stream->pos() - pat_pos);
	stream->write4bytes(CRC_32);

	return ret;
}

int tspsiplayload::size()
{
	int sz = 0;

	// section size is the sl plus the crc32, 4 bytes is crc value
	section_length = psi_size() + 4;

	sz += m_packet->payload_unit_start_indicator ? 1 : 0;   //plus 1 for point_value
	sz += 3;
	sz += section_length;

	return sz;
}

tspatprogram::tspatprogram(int pmtnumber, int pmtpid)
{
	number = pmtnumber;
	pid = pmtpid;
	const1_value = 7;
}

tspatprogram::~tspatprogram()
{
	
}

int tspatprogram::encode(streamworker* stream)
{
	int ret = 0;

	// atleast 4B for PAT program specified
	if (!stream->require(4)) {
		ret = -1;
		printf("ts: mux PAT failed.\n");
		return ret;
	}

	int tmpv = pid & 0x1FFF;
	tmpv |= (number << 16) & 0xFFFF0000;
	tmpv |= (const1_value << 13) & 0xE000;
	stream->write4bytes(tmpv);

	return ret;
}

tspmtplayload::tspmtplayload(tspacket* pkt) :tspsiplayload(pkt)
{
	const1_value0 = 3;
	const1_value1 = 7;
	const1_value2 = 0x0f;
	program_info_length = 0;
	program_info_desc = NULL;
}

tspmtplayload::~tspmtplayload()
{

}


tspatplayload::tspatplayload(tspacket* pkt) :tspsiplayload(pkt)
{
	const1_value = 3;
}

tspatplayload::~tspatplayload()
{

}

int tspatplayload::psi_encode(streamworker* stream)
{
	int ret = 0;

	// atleast 5B for PAT specified
	if (!stream->require(5)) {
		ret = -1;
		printf("ts: mux PAT failed.\n");
		return ret;
	}

	// 2B
	stream->write2bytes(transport_stream_id);

	// 1B
	char cniv = current_next_indicator & 0x01;
	cniv |= (version_number << 1) & 0x3E;
	cniv |= (const1_value << 6) & 0xC0;
	stream->write1bytes(cniv);

	// 1B
	stream->write1bytes(section_number);
	// 1B
	stream->write1bytes(last_section_number);

	// multiple 4B program data.
	for (int i = 0; i < (int)programs.size(); i++) {
		tspatprogram* program = programs.at(i);
		if ((ret = program->encode(stream)) != 0) {
			return ret;
		}
		// update the apply pid table.
		//packet->context->set(program->pid, SrsTsPidApplyPMT);
	}

	// update the apply pid table.
	//packet->context->set(packet->pid, SrsTsPidApplyPAT);

	return ret;
}

int tspatplayload::psi_size()
{
	/*
		5bites:
		transport_stream_id  16  uimsbf
		reserved  2  bslbf
		version_number  5  uimsbf
		current_next_indicator  1  bslbf
		section_number  8  uimsbf
		last_section_number  8  
	*/
	int sz = 5;
	for (int i = 0; i < (int)programs.size(); i++) {
		tspatprogram* program = programs.at(i);
		sz += program->size();
	}
	return sz;
}

int tspmtplayload::psi_size()
{
	int sz = 9;
	sz += program_info_length;
	for (int i = 0; i < (int)infos.size(); i++) {
		tsplayloadesinfo* info = infos.at(i);
		sz += info->size();
	}
	return sz;
}

int tspmtplayload::psi_encode(streamworker* stream)
{
	int ret = 0;

	// atleast 9B for PMT specified
	if (!stream->require(9)) {
		ret = -1;
		printf("ts: mux PMT failed.\n", ret);
		return ret;
	}

	// 2B
	stream->write2bytes(program_number);

	// 1B
	char cniv = current_next_indicator & 0x01;
	cniv |= (const1_value0 << 6) & 0xC0;
	cniv |= (version_number << 1) & 0xFE;
	stream->write1bytes(cniv);

	// 1B
	stream->write1bytes(section_number);

	// 1B
	stream->write1bytes(last_section_number);

	// 2B
	short ppv = PCR_PID & 0x1FFF;
	ppv |= (const1_value1 << 13) & 0xE000;
	stream->write2bytes(ppv);

	// 2B
	short pilv = program_info_length & 0xFFF;
	pilv |= (const1_value2 << 12) & 0xF000;
	stream->write2bytes(pilv);

	if (program_info_length > 0) {
		if (!stream->require(program_info_length)) {
			ret = -1;
			printf("ts: mux PMT program info failed\n");
			return ret;
		}

		stream->write_stream(program_info_desc, program_info_length);
	}

	for (int i = 0; i < (int)infos.size(); i++) {
		tsplayloadesinfo* info = infos.at(i);
		if ((ret = info->encode(stream)) != 0) {
			return ret;
		}
	}

}

tssegment::tssegment() :vcid(7),m_segmentfile(NULL)
{
	if (!m_segmentfile)
	{
		string segmentname = "1.ts";
		m_segmentfile = fopen(segmentname.c_str(),"wb");
	}
}

tssegment::~tssegment()
{

}

int tssegment::write_direct_stream(tspacket* pkt ,streamworker* stream)
{
	int ret = 0;
	ret = pkt->encode_packet_data(stream);
	if (ret != 0)
		return ret;
	ret = flush_file(stream);
	if (ret != 0)
		return ret;
	return ret;
}

int tssegment::close_tssegment()
{
	if (!m_segmentfile)
		return -1;
	fclose(m_segmentfile);
}

int tssegment::flush_file(streamworker* stream)
{
	if (!m_segmentfile)
	{
		printf("segment hasn't created\n");
		return -1;
	}
	int num = fwrite(stream->data(),1,stream->getsize(),m_segmentfile);
	if (num != stream->getsize())
	{
		printf("write error\n");
		return -1;
	}
	return num;
}

int tssegment::write_data(tspacket* pkt)
{
	int ret = 0;
	char* buf = new char[TS_PACKET_SIZE];
	memset(buf, 0, TS_PACKET_SIZE);
	int nb_buf = pkt->size();
	assert(nb_buf <= TS_PACKET_SIZE);
	// set the left bytes with 0xFF.
	memset(buf + nb_buf, 0xFF, TS_PACKET_SIZE - nb_buf);
	streamworker stream;
	stream.init(buf, TS_PACKET_SIZE);
	ret = pkt->encode_packet_data(&stream);
	if (ret != 0)
		return ret;
	ret = flush_file(&stream);
	if (ret != 0)
		return ret;
	return ret;
}

int tssegment::encode_pat_pmt()
{

}

int tssegment::write_segment(char* buf, int length)
{
	int ret = 0;
	unsigned char fresh_nalu_header[] = { 0x00, 0x00, 0x00, 0x01 };
	unsigned char cont_nalu_header[] = { 0x00, 0x00, 0x01 };



}


tsplayloadesinfo::tsplayloadesinfo()
{
	stream_type = TsStreamVideoH264;   //目前ts文件只是有264的video
	const1_value0 = 7;
	elementary_PID = TS_VIDEO_AVC_PID;
	const1_value1 = 0x0f;
	ES_info_length = 0;
	ES_info = NULL;
}

tsplayloadesinfo::~tsplayloadesinfo()
{

}

int tsplayloadesinfo::size()
{
	int sz = 5;
	sz += ES_info_length;
	return sz;
}

int tsplayloadesinfo::encode(streamworker* stream)
{
	int ret = 0;

	// 5B
	if (!stream->require(5)) {
		ret = -1;
		printf("ts: mux PMT es info failed.\n");
		return ret;
	}

	stream->write1bytes(stream_type);

	short epv = elementary_PID & 0x1FFF;
	epv |= (const1_value0 << 13) & 0xE000;
	stream->write2bytes(epv);

	short eilv = ES_info_length & 0x0FFF;
	eilv |= (const1_value1 << 12) & 0xF000;
	stream->write2bytes(eilv);

	if (ES_info_length > 0) {
		if (!stream->require(ES_info_length)) {
			ret = -1;
			printf("ts: mux PMT es info data failed.\n");
			return ret;
		}
		stream->write_stream(ES_info, ES_info_length);
	}

	return ret;
	
}

tspacket::tspacket()
{

}

tspacket::~tspacket()
{

}

int tspacket::encode_packet_data(streamworker* stream)
{
	stream->write1bytes(sync_byte);
	short pidv = pid & 0x1FFF;
	pidv |= (transport_priority << 13) & 0x2000;
	pidv |= (transport_error_indicator << 15) & 0x8000;
	pidv |= (payload_unit_start_indicator << 14) & 0x4000;
	stream->write2bytes(pidv);

	char ccv = continuity_counter & 0x0F;
	ccv |= (transport_scrambling_control << 6) & 0xC0;
	ccv |= (adaption_field_control << 4) & 0x30;
	stream->write1bytes(ccv);
	if (adaptation_field) {
		//目前没有adaptation_field
	}
	if (playload) {
		if ((playload->encode(stream)) != 0) {
			printf("ts: mux payload failed.\n");
			return -1;
		}
		printf("ts: mux payload ok.\n");
	}
	return 0;
}

int tspacket::size()
{
	int size = 0;
	/*
	sync_byte
	8  bslbf
	transport_error_indicator
	1  bslbf
	payload_unit_start_indicator  1  bslbf
	transport_priority  1  bslbf
	PID  13  uimsbf
	transport_scrambling_control  2  bslbf
	adaptation_field_control  2  bslbf
	continuity_counter
	*/
	size += 4;
	//adaptation_field 目前的值是0
	size += 0;
	//
	size += playload->size();
	return size;
	
}

tsmuxer::tsmuxer() :m_avccodec(NULL), m_segment(NULL),m_continuity_counter(0)
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
	avcsample sample;
	int ret = m_avccodec->avcdemux(data, length, sample);
	if (ret == -1)
		return -1;
	if (ret == -2)   //目前获取到的是sps 和pps 不需要在这里直接写进ts
		return 0;
	if (m_avccodec->do_cache_avc_format(sample, m_videoplayload) == -1)
		return -1;
	if (!m_segment)
	{
		m_segment = new tssegment();

		//write PAT table
		tspacket *pkt = create_pat_packet(TS_PMT_NUMBER, TS_PMT_PID);
		m_segment->write_data(pkt);
		delete pkt;

		//write PMT table
		pkt = create_pmt_packet(TS_PMT_NUMBER);
		m_segment->write_data(pkt);
		//m_segment->close_tssegment();
	}
	else
	{
		char *p = m_videoplayload.data();
		char *start = p;
		char *end = p + m_videoplayload.size();
		while (start != end)
		{
			tspacket *pkt;
			//没有创建chunksgement就说明对AV数据进行tsmuxer
			if (start == p)
			{
				pkt = create_first_pes(m_videoplayload.size());
			}
			else
			{
				pkt = create_continue_pes();
			}
			//将video的数据分包进pes包，并且写入tsmuxer
			int nb_buf = pkt->size();
			int left = TS_PACKET_SIZE - nb_buf; //剩余的ts Packet大小用于存放Video实体数据
			char* buf = new char[TS_PACKET_SIZE];
			memcpy(buf + nb_buf, start, left);
			start += left;
			streamworker stream;
			stream.init(buf, TS_PACKET_SIZE);
			m_segment->write_direct_stream(pkt, &stream);  //写pes数据
		}
		
	}
}

tspacket* tsmuxer::create_first_pes(int size)
{
	tspacket* pkt = new tspacket();
	pkt->sync_byte = 0x47;
	pkt->transport_error_indicator = 0;
	pkt->payload_unit_start_indicator = 1;
	pkt->transport_priority = 0;
	pkt->pid = TS_VIDEO_AVC_PID;
	pkt->transport_scrambling_control = TsScrambledDisabled;
	pkt->adaption_field_control = TsAdaptationFieldTypePayloadOnly;
	pkt->continuity_counter = m_continuity_counter;
	pkt->adaptation_field = NULL;
	tspesplayload* pes = new tspesplayload(pkt);
	pkt->playload = pes;

	//init pes playload
	pes->packet_start_code_prefix = 0x01;
	pes->stream_id = (int)TS_VIDEO_PES_STREAMID;
	pes->PES_packet_length = size > 0xffff ? 0 : size;
	pes->PES_scrambling_control = 0;
	pes->PES_priority = 0;
	pes->data_alignment_indicator = 0;
	pes->copyright = 0;
	pes->original_or_copy = 0;
	pes->PTS_DTS_flags = 0x02;
	pes->ESCR_flag = 0;
	pes->ES_rate_flag = 0;
	pes->DSM_trick_mode_flag = 0;
	pes->additional_copy_info_flag = 0;
	pes->PES_CRC_flag = 0;
	pes->PES_extension_flag = 0;
	pes->PES_header_data_length = 0; // calc in size.

	return pkt;
}
tspacket* tsmuxer::create_continue_pes()
{
	tspacket* pkt = new tspacket();
	pkt->sync_byte = 0x47;
	pkt->transport_error_indicator = 0;
	pkt->payload_unit_start_indicator = 0;
	pkt->transport_priority = 0;
	pkt->pid = TS_VIDEO_AVC_PID;
	pkt->transport_scrambling_control = TsScrambledDisabled;
	pkt->adaption_field_control = TsAdaptationFieldTypePayloadOnly;
	pkt->continuity_counter = m_continuity_counter;
	pkt->adaptation_field = NULL;
	pkt->playload = NULL;

	return pkt;
}



tspacket* tsmuxer::create_pmt_packet(int pmtnumber)
{
	tspacket* pkt = new tspacket();
	pkt->sync_byte = 0x47;
	pkt->transport_error_indicator = 0;
	pkt->payload_unit_start_indicator = 1;
	pkt->transport_priority = 0;
	pkt->pid = TS_PMT_PID;
	pkt->transport_scrambling_control = TsScrambledDisabled;
	pkt->adaption_field_control = TsAdaptationFieldTypePayloadOnly;
	// TODO: FIXME: maybe should continuous in channel.
	pkt->continuity_counter = 0;
	pkt->adaptation_field = NULL;
	tspmtplayload* pmt = new tspmtplayload(pkt);
	pkt->playload = pmt;

	pmt->pointer_field = 0;
	pmt->table_id = TsPsiIdPms;
	pmt->section_syntax_indicator = 1;
	pmt->section_length = 0; // calc in size.
	pmt->program_number = pmtnumber;
	pmt->version_number = 0;
	pmt->current_next_indicator = 1;
	pmt->section_number = 0;
	pmt->last_section_number = 0;
	pmt->program_info_length = 0;
	pmt->infos.push_back(new tsplayloadesinfo());
	pmt->CRC_32 = 0;

	return pkt;
}

tspacket* tsmuxer::create_pat_packet(int pmtnumber, int pmtpid)
{
	tspacket* pkt = new tspacket();
	pkt->sync_byte = 0x47;
	pkt->transport_error_indicator = 0;
	pkt->payload_unit_start_indicator = 1;
	pkt->transport_priority = 0;
	pkt->pid = TsPidPAT;
	pkt->transport_scrambling_control = TsScrambledDisabled;
	pkt->adaption_field_control = TsAdaptationFieldTypePayloadOnly;
	pkt->continuity_counter = 0;
	pkt->adaptation_field = NULL;
	tspatplayload* pat = new tspatplayload(pkt);
	pkt->playload = pat;
	
	//初始化pat数据结构
	pat->pointer_field = 0;
	pat->table_id = TsPsiIdPas;
	pat->section_syntax_indicator = 1;
	pat->section_length = 0; // calc in size.
	pat->transport_stream_id = 1;
	pat->version_number = 0;
	pat->current_next_indicator = 1;
	pat->section_number = 0;
	pat->last_section_number = 0;
	pat->programs.push_back(new tspatprogram(pmtnumber, pmtpid));
	pat->CRC_32 = 0; // calc in encode.
	return pkt;
}

int tsmuxer::cache_video(avcsample& sample, vector<char>& video)
{
	
}

int tsmuxer::write_video(avcsample& sample)
{
	
}