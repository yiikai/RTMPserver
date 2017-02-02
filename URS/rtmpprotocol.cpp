#include "rtmpprotocol.h"
#include "ioutility.h"
#include "utilityfunc.h"
#include "amf0.h"
#include "cycleepoll.h"
#include "rtmpcomplexhandshake.h"
#include "flvcodec.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

int rtmpcreatestreampacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);
	if ((ret = srs_amf0_read_string(&stream, command_name)) != 0) {
		printf("amf0 decode create stream command_name failed. ret=%d\n", ret);
		return ret;
	}

	if (command_name.empty() || command_name != RTMP_AMF0_COMMAND_CREATE_STREAM) {
		ret = -1;
		printf("amf0 decode create stream packet command_name failed. "
				"command_name=%s, ret=%d\n", command_name.c_str(), ret);
		return ret;
	}
	if ((ret = srs_amf0_read_number(&stream, transaction_id)) != 0) {
		printf("amf0 decode create stream packet transaction_id failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_null(&stream)) != 0) {
		printf("amf0 decode create stream packet command_object failed. ret=%d\n", ret);
		return ret;
	}

	return ret;
}

int rtmpcreatestreamrespacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	if ((ret = srs_amf0_write_number(stream, transaction_id)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = srs_amf0_write_null(stream)) != 0) {
		printf("encode command_object failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_object success.\n");

	if ((ret = srs_amf0_write_number(stream, stream_id)) != 0) {
		printf("encode args failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode args success.\n");

	printf("encode FMLE start response packet success.\n");

	return ret;
}

int rtmpfcpublishpacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);
	if ((ret = srs_amf0_read_string(&stream, command_name)) != 0) {
		printf("amf0 decode FCPublish packet command_name failed. ret=%d\n", ret);
		return ret;
	}
	if (command_name.empty() || command_name != RTMP_AMF0_COMMAND_FC_PUBLISH) {
		ret = -1;
		printf("amf0 decode FCPublish packet command_name failed. "
				"command_name=%s, ret=%d\n", command_name.c_str(), ret);
		return ret;
	}
	if ((ret = srs_amf0_read_number(&stream, transaction_id)) != 0) {
		printf("amf0 decode FCPublish packet  transaction_id failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_null(&stream)) != 0) {
		printf("amf0 decode FCPublish packet  command_object failed. ret=%d", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_string(&stream, stream_name)) != 0) {
		printf("amf0 decode FCPublish packet stream_name failed. ret=%d\n", ret);
		return ret;
	}
	return ret;
}

int rtmpsetchunksizepacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);
	int value = stream.read_4bytes();
	printf("client set chunksize is %d\n", value);
	setchunksize = value;
	return ret;
}


int rtmpfcpublishrespacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	if ((ret = srs_amf0_write_number(stream, transaction_id)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = srs_amf0_write_null(stream)) != 0) {
		printf("encode command_object failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_object success.");

	if ((ret = srs_amf0_write_undefined(stream)) != 0) {
		printf("encode args failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode args success.\n");

	printf("encode FMLE start response packet success.\n");

	return ret;
}

int rtmpreleasestreampacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);
	if ((ret = srs_amf0_read_string(&stream, command_name)) != 0) {
		printf("amf0 decode FMLE start response command_name failed. ret=%d\n", ret);
		return ret;
	}
	if (command_name.empty() || command_name != RTMP_AMF0_COMMAND_RELEASE_STREAM) {
		ret = -1;
		printf("amf0 decode FMLE start response command_name failed. "
				"command_name=%s, ret=%d\n", command_name.c_str(), ret);
		return ret;
	}

	if ((ret = srs_amf0_read_number(&stream, transaction_id)) != 0) {
		printf("amf0 decode FMLE start response transaction_id failed. ret=%d\n", ret);
		return ret;
	}

	printf("amf0 decode FMLE start packet success\n");

	return ret;
}


int rtmppublishpacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);
	if ((ret = srs_amf0_read_string(&stream, command_name)) != 0) {
		printf("amf0 decode publish packet command_name command_name failed. ret=%d\n", ret);
		return ret;
	}
	if (command_name.empty() || command_name != RTMP_AMF0_COMMAND_PUBLISH) {
		ret = -1;
		printf("amf0 decode publish packet command_name failed. "
				"command_name=%s, ret=%d\n", command_name.c_str(), ret);
		return ret;
	}

	if ((ret = srs_amf0_read_number(&stream, transaction_id)) != 0) {
		printf("amf0 decode publish packet transaction_id failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_null(&stream)) != 0) {
		printf("amf0 decode publish packet NULL failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_string(&stream, stream_name)) != 0) {
		printf("amf0 decode publish packet stream_name failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_string(&stream, app_name)) != 0) {
		printf("amf0 decode publish packet app_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("amf0 decode FMLE start packet success\n");

	return ret;
}

int rtmponbwdonepacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	if ((ret = srs_amf0_write_number(stream, transaction_id)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = srs_amf0_write_null(stream)) != 0) {
		printf("encode args failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode args success.\n");

	printf("encode onBWDone packet success.\n");

	return ret;
}

int rtmppublishrespacket::encode_packet(streamworker* stream)
{
	int ret = 0;
	if ((ret = srs_amf0_write_string(stream, "onFCPublish")) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	/*onstatus packet need set transcodeID 0*/
	if ((ret = srs_amf0_write_number(stream, 0)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = srs_amf0_write_null(stream)) != 0)
	{
		printf("encode NULL failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = command_object->write(stream)) != 0) {
		printf("encode data failed. ret=%d\n", ret);
		return ret;
	}

	return ret;
}

int rtmponstatusdatapacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	if ((ret = data->write(stream)) != 0) {
		printf("encode data failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode data success.\n");

	printf("encode onStatus(Data) packet success.\n");

	return ret;
}


int rtmpcommonpacket::encode(int& size, char** ppayload)
{
	int ret = 0;

	size = get_size();
	char* payload = NULL;

	streamworker stream;

	if (size > 0) {
		payload = new char[size];
		stream.init(payload, size);
	}

	if ((ret = encode_packet(&stream)) != 0) {
		printf("encode the packet failed. ret=%d\n", ret);
		return ret;
	}

	*ppayload = payload;
	printf("encode the packet success. size=%d\n", size);

	return ret;
}


setwindowacksizepacket::setwindowacksizepacket() :rtmpcommonpacket(),
	ackowledgement_window_size(0)
{

}

setwindowacksizepacket::~setwindowacksizepacket()
{

}

int setwindowacksizepacket::get_size()
{
	return 4;
}

int setwindowacksizepacket::get_message_type()
{
	return RTMP_MSG_WindowAcknowledgementSize;
}

int setwindowacksizepacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if (!stream->require(4)) {
		ret = -1;
		printf("encode ack size packet failed. ret=%d\n", ret);
		return ret;
	}

	stream->write4bytes(ackowledgement_window_size);

	printf("encode ack size packet "
			"success. ack_size=%d\n", ackowledgement_window_size);

	return ret;
}

setpeerbandwidthpacket::setpeerbandwidthpacket()
{

}

setpeerbandwidthpacket::~setpeerbandwidthpacket()
{

}

int setpeerbandwidthpacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if (!stream->require(4)) {
		ret = -1;
		printf("encode peer bandwidth size packet failed. ret=%d\n", ret);
		return ret;
	}
	stream->write4bytes(bandwidth);
	if (!stream->require(1)) {
		ret = -1;
		printf("encode peer bandwidth size packet type failed. ret=%d\n", ret);
		return ret;
	}
	stream->write1bytes(type);
	printf("encode peer bandwidth packet "
			"success. bandwidth=%d, type=%d\n", bandwidth, type);
	return ret;
}

int setchunksizepacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if (!stream->require(4)) {
		ret = -1;
		printf("encode set chunk size packet failed. ret=%d\n", ret);
		return ret;
	}
	stream->write4bytes(size);
	printf("encode set chunk size packet "
			"success. size=%d\n", size);
	return ret;
}


int rtmpplaypacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);

	if ((ret = srs_amf0_read_string(&stream, command_name)) != 0) {
		printf("amf0 decode play command_name failed. ret=%d\n", ret);
		return ret;
	}
	if (command_name.empty() || command_name != RTMP_AMF0_COMMAND_PLAY) {
		ret = -1;
		printf("amf0 decode play command_name failed. "
				"command_name=%s, ret=%d\n", command_name.c_str(), ret);
		return ret;
	}

	if ((ret = srs_amf0_read_number(&stream, transaction_id)) != 0) {
		printf("amf0 decode play transaction_id failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_null(&stream)) != 0) {
		printf("amf0 decode play command_object failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_string(&stream, stream_name)) != 0) {
		printf("amf0 decode play stream_name failed. ret=%d\n", ret);
		return ret;
	}

	if (!stream.isempty() && (ret = srs_amf0_read_number(&stream, start)) != 0) {
		printf("amf0 decode play start failed. ret=%d\n", ret);
		return ret;
	}
	if (!stream.isempty() && (ret = srs_amf0_read_number(&stream, duration)) != 0) {
		printf("amf0 decode play duration failed. ret=%d\n", ret);
		return ret;
	}

	if (stream.isempty()) {
		return ret;
	}

	Amf0Any* reset_value = NULL;
	if ((ret = srs_amf0_read_any(&stream, &reset_value)) != 0) {
		ret = -1;
		printf("amf0 read play reset marker failed. ret=%d\n", ret);
		return ret;
	}
	reset = ((Amf0boolean*)(reset_value))->value;
	printf("flush play list previous is %d\n", reset);
#if 0
	if (reset_value) {
		// check if the value is bool or number
		// An optional Boolean value or number that specifies whether
		// to flush any previous playlist
		if (reset_value->is_boolean()) {
			reset = reset_value->to_boolean();
		} else if (reset_value->is_number()) {
			reset = (reset_value->to_number() != 0);
		} else {
			ret = ERROR_RTMP_AMF0_DECODE;
			srs_error("amf0 invalid type=%#x, requires number or bool, ret=%d", reset_value->marker, ret);
			return ret;
		}
	}
#endif
	printf("amf0 decode play packet success\n");

	return ret;
}



int rtmpOnStatusCallPacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, RTMP_AMF0_COMMAND_ON_STATUS)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	if ((ret = srs_amf0_write_number(stream, transaction_id)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = srs_amf0_write_null(stream)) != 0) {
		printf("encode args failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode args success.\n");;

	if ((ret = data->write(stream)) != 0) {
		printf("encode data failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode data success.\n");

	printf("encode onStatus(Call) packet success.\n");

	return ret;
}

int rtmpUnPublishPacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);

	if ((ret = srs_amf0_read_string(&stream, command_name)) != 0) {
		printf("amf0 decode FMLE start command_name failed. ret=%d", ret);
		return ret;
	}
	if (command_name.empty() || (command_name != RTMP_AMF0_COMMAND_UNPUBLISH))
	{
		ret = -1;
		printf("amf0 decode unpublish command_name failed.\n "
				"command_name=%s, ret=%d", command_name.c_str(), ret);
		return ret;
	}

	if ((ret = srs_amf0_read_number(&stream, transaction_id)) != 0) {
		printf("amf0 decode unpublish transaction_id failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_null(&stream)) != 0) {
		printf("amf0 decode unpublish command_object failed. ret=%d\n", ret);
		return ret;
	}

	if ((ret = srs_amf0_read_string(&stream, stream_name)) != 0) {
		printf("amf0 decode unpublish stream_name failed. ret=%d\n", ret);
		return ret;
	}

	printf("amf0 decode unpublish packet success\n");

	return ret;
}

int rtmpconnectpacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);

	if (srs_amf0_read_string(&stream, command_name) == -1)
		return -1;
	if (command_name.empty() || command_name != RTMP_AMF0_COMMAND_CONNECT) {
		ret = -1;
		printf("amf0 decode connect command_name failed. "
				"command_name=%s, ret=%d\n", command_name.c_str(), ret);
		return ret;
	}
	if ((ret = srs_amf0_read_number(&stream, transaction_id)) != 0) {
		printf("amf0 decode connect transaction_id failed. ret=%d", ret);
		return ret;
	}
	if (transaction_id != 1)
	{
		ret = -1;
		printf("connect transactionid shoud be 1 actually is %d\n", transaction_id);
		return ret;
	}

	if ((ret = command_object->read(&stream)) != 0) {
		printf("amf0 decode connect command_object failed. ret=%d\n", ret);
		return ret;
	}

	printf("amf0 decode connect packet success\n");

	return ret;

}
#if 0
int onstatusrespacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.");

	if ((ret = srs_amf0_write_number(stream, transaction_id)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = srs_amf0_write_null(stream)) != 0) {
		printf("encode args failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode args success.\n");;

	if ((ret = data->write(stream)) != 0) {
		printf("encode data failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode data success.\n");

	printf("encode onStatus(Call) packet success.\n");

	return ret;
}
#endif

int rtmpusercontrolmessage::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);

	if (!stream.require(2)) {
		ret = -1;
		printf("decode user control failed. ret=%d\n", ret);
		return ret;
	}

	event_type = stream.read_2bytes();

	if (event_type == SrsPCUCFmsEvent0) {
		if (!stream.require(1)) {
			ret = -1;
			printf("decode user control failed. ret=%d\n", ret);
			return ret;
		}
		event_data = stream.read_1byte();
	}
	else {
		if (!stream.require(4)) {
			ret = -1;
			printf("decode user control failed. ret=%d\n", ret);
			return ret;
		}
		event_data = stream.read_4bytes();
	}

	if (event_type == SrcPCUCSetBufferLength) {
		if (!stream.require(4)) {
			ret = -1;
			printf("decode user control packet failed. ret=%d\n", ret);
			return ret;
		}
		extra_data = stream.read_4bytes();
	}

	printf("decode user control success. "
			"event_type=%d, event_data=%d, extra_data=%d\n",
			event_type, event_data, extra_data);

	return ret;
}


int rtmpreleasestreamresponsepacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	if ((ret = srs_amf0_write_number(stream, transaction_id)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = srs_amf0_write_null(stream)) != 0) {
		printf("encode args NULL failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode args NULL success.\n");

	if ((ret = srs_amf0_write_undefined(stream)) != 0) {
		printf("encode args undefine  failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode args undefine success.\n");

	printf("encode FMLE start response packet success.\n");

	return ret;
}

int rtmpconnectresponsepacket::encode_packet(streamworker* stream)
{
	int ret = 0;
	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}

	printf("encode command_name success.\n");

	if ((ret = srs_amf0_write_number(stream, transaction_id)) != 0) {
		printf("encode transaction_id failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode transaction_id success.\n");

	if ((ret = m_prop->write(stream)) != 0) {
		printf("encode props failed. ret=%d\n", ret);
		return ret;
	}

	printf("encode props success.\n");

	if ((ret = m_info->write(stream)) != 0) {
		printf("encode info failed. ret=%d\n", ret);
		return ret;
	}

	printf("encode info success.\n");

	printf("encode connect app response packet success.\n");
	return ret;
}

int rtmpsampleacesspacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, command_name)) != 0) {
		printf("encode command_name failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode command_name success.\n");

	if ((ret = srs_amf0_write_boolean(stream, video_sample_access)) != 0) {
		printf("encode video_sample_access failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode video_sample_access success.\n");

	if ((ret = srs_amf0_write_boolean(stream, audio_sample_access)) != 0) {
		printf("encode audio_sample_access failed. ret=%d\n", ret);
		return ret;
	}
	printf("encode audio_sample_access success.\n");;

	printf("encode |RtmpSampleAccess packet success.\n");

	return ret;
}


int usercontrolpacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if (!stream->require(get_size())) {
		ret = -1;
		printf("encode user control packet failed. ret=%d\n", ret);
		return ret;
	}

	stream->write2bytes(event_type);

	if (event_type == SrsPCUCFmsEvent0) {
		stream->write1bytes(event_data);
	}
	else {
		stream->write4bytes(event_data);
	}

	// when event type is set buffer length,
	// write the extra buffer length.
	if (event_type == SrcPCUCSetBufferLength) {
		stream->write4bytes(extra_data);
		printf("user control message, buffer_length=%d\n", extra_data);
	}

	printf("encode user control packet success. "
			"event_type=%d, event_data=%d\n", event_type, event_data);

	return ret;
}


char* m_c0c1;
	char* m_s0s1s2;
	char* m_c2;

rtmpprotocol::rtmpprotocol(int fd) :m_source(NULL), 
									default_chunksize(128),
									m_c0c1(NULL),
									m_s0s1s2(NULL),
									m_c2(NULL),
									m_tsmuxer(NULL)
{
	m_fd = fd;
	m_tsmuxer = new tsmuxer();
	//m_clientVec.reserve(10);
}

rtmpprotocol::~rtmpprotocol()
{

}


int rtmpprotocol::readC0C1()
{
	m_c0c1 = new char[1537];
	memset(m_c0c1, 0, 1537);
	int ret = ioutility::ioread(m_fd, m_c0c1, 1537);
	if (ret != 1537)
	{
		printf("read error\n");
		return -1;
	}
	printf("read buf %d\n", m_c0c1[0]);
	if (m_c0c1[0] != 0x03)
	{
		printf("not support this version of ");
		return 0;
	}
	return 0;
}

int rtmpprotocol::handle_handshakeC2()
{
	int ret;
	readC2();
	c2s2 c2;
	c2.parse(getC2(), 1536);

	printf("complex handshake read c2 success.\n");

	// verify c2
	// never verify c2, for ffmpeg will failed.
	// it's ok for flash.
	printf("complex handshake success\n");
}

int rtmpprotocol::handle_handshakeC0C1()
{
	readC0C1();
	c1s1 c1;
	Hschem sch = HAND_SHAKE_SCHEMA0;
	c1.parse(getC0C1() + 1, 1536, sch);
	bool is_valid = false;
	int ret;
	HandType type = complex;
	setHandShakeMethod(type);
	if ((ret = c1.c1_validate_digest(is_valid)) != 0 || !is_valid)
	{
		printf("schema0 failed, try schema1.\n");
		sch = HAND_SHAKE_SCHEMA1;
		c1.parse(getC0C1() + 1, 1536, sch);
		if ((ret = c1.c1_validate_digest(is_valid)) != 0 || !is_valid) {
			//ret = ERROR_RTMP_TRY_SIMPLE_HS;
			HandType type = simple;
			setHandShakeMethod(type);
			printf("all schema valid failed, try simple handshake. ret=%d", ret);
		}
		HandType type = complex;
		setHandShakeMethod(type);
	}

	//after read c0c1 need create s0s1s2 send to client
	createS0S1S2(c1);
}

int rtmpprotocol::createS0S1S2(c1s1& _c1s1)
{
	if (m_handtype == simple)
	{
		if (m_s0s1s2)
			return 0;
		m_s0s1s2 = new char[3073];
		random_generate(m_s0s1s2, 3073);
		streamworker stream;
		stream.init(m_s0s1s2, 3073);
		stream.write1bytes(0x03);   //RTMP server select version 3
		stream.write4bytes((int)::time(NULL));
		stream.write4bytes(0);     //Zero (4 bytes): This field MUST be all 0s.
		if (m_c0c1) {                                //The C2 and S2 packets are 1536 octets long, 
			memcpy(m_s0s1s2 + 1537, m_c0c1 + 1, 1536);   //and nearly an echo of S1and C1 (respectively),       									   
		}
		int ret = ioutility::iowrite(m_fd, m_s0s1s2, 3073);
		if (ret != 3073)
			return -1;
	}
	else
	{
		bool is_valid = false;
		int ret;
		c1s1 s1;
		if ((ret = s1.s1_create(&_c1s1)) != 0) {
			printf("create s1 from c1 failed. ret=%d\n", ret);
			return ret;
		}
		printf("create s1 from c1 success.\n");

		if ((ret = s1.s1_validate_digest(is_valid)) != 0 || !is_valid) {
			ret = -1;
			printf("verify s1 failed, try simple handshake. ret=%d\n", ret);
			return ret;
		}
		c2s2 s2;
		if ((ret = s2.s2_create(&_c1s1)) != 0) {
			printf("create s2 from c1 failed. ret=%d\n", ret);
			return ret;
		}
		printf("create s2 from c1 success.\n");
		if ((ret = s2.s2_validate(&_c1s1, is_valid)) != 0 || !is_valid) {
			printf("verify s2 failed. ret=%d\n", ret);
			return ret;
		}
		printf("verify s2 success.\n");

		if (m_s0s1s2)
			return 0;
		m_s0s1s2 = new char[3073];
		random_generate(m_s0s1s2, 3073);
		streamworker stream;
		stream.init(m_s0s1s2, 9);
		stream.write1bytes(0x03);
		stream.write4bytes((int)::time(NULL));
		// s1 time2 copy from c1
		if (m_c0c1)
			stream.write_stream(m_c0c1 + 1, 4);
		if ((ret = s1.dump(m_s0s1s2 + 1, 1536)) != 0) {
			return ret;
		}
		if ((ret = s2.dump(m_s0s1s2 + 1537, 1536)) != 0) {
			return ret;
		}

		ret = ioutility::iowrite(m_fd, m_s0s1s2, 3073);
		if (ret != 3073)
			return -1;

	}
	return 0;
}



int rtmpprotocol::readC2()
{
	if (m_c2)
		return 0;
	m_c2 = new char[1536];
	memset(m_c0c1, 0, 1536);
	int needread = 1536;
	int hasread = 0;
	do{
		int ret = ioutility::ioread(m_fd, m_c2 + hasread, needread);
		hasread += ret;
		needread -= ret;
	} while (needread != 0);

	if (hasread != 1536)
	{
		printf("read error\n");
		return -1;
	}
	return 0;
}

int rtmpprotocol::connectapp()
{
	rtmpcommonmessage* msg = new rtmpcommonmessage();
	recv_message(&msg);
	set_window_ack_size((int)(2.5 * 1000 * 1000));
	set_peer_bandwidth_size((int)(2.5 * 1000 * 1000), 2);
	if (default_chunksize == 128)
	{
		printf("may be client not set chunksize for server, and to make paly client happy need set chnksize to 4096\n");
		set_chunk_size(4096);
	}
	else
	{
		set_chunk_size(default_chunksize);
	}

	printf("set chunksize from client is %d\n", default_chunksize);
	response_connect_app();
	set_bwdown();
	delete msg;
	return 0;
}

int rtmpprotocol::set_window_ack_size(int ack_size)
{
	int ret = 0;
	setwindowacksizepacket* pkt = new setwindowacksizepacket();
	pkt->ackowledgement_window_size = ack_size;
	if ((ret = send_and_free_packet(pkt, 0)) != 0) //control message cmd must be stream id 0
	{
		printf("set window ack size failed\n");
		return -1;
	}
	printf("send ack size message success. ack_size=%d\n", ack_size);
	return ret;
}

int rtmpprotocol::set_peer_bandwidth_size(int bandwidth, char type)
{
	int ret = 0;
	setpeerbandwidthpacket* pkt = new setpeerbandwidthpacket();
	pkt->bandwidth = bandwidth;
	pkt->type = type;
	if ((ret = send_and_free_packet(pkt, 0)) != 0) //control message cmd must be stream id 0
	{
		printf("set peerband width size failed\n");
		return -1;
	}
	printf("send peerband width size message success. bandwidth=%d\n", bandwidth);
	return ret;
}

int rtmpprotocol::set_bwdown()
{
	int ret = 0;
	rtmponbwdonepacket* pkt = new rtmponbwdonepacket();
	if ((ret = send_and_free_packet(pkt, 0)) != 0) //control message cmd must be stream id 0
	{
		printf("set bw done size failed\n");
		return -1;
	}
}

int rtmpprotocol::set_chunk_size(int size)
{
	int ret = 0;
	setchunksizepacket* pkt = new setchunksizepacket();
	pkt->size = size;
	if ((ret = send_and_free_packet(pkt, 0)) != 0) //control message cmd must be stream id 0
	{
		printf("set chunk size failed\n");
		return -1;
	}
	printf("send chunk size message success. size=%d\n", size);
	return ret;
}

int rtmpprotocol::distinguishusercontrolmessage(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);
	rtmpusercontrolmessage* packet = new rtmpusercontrolmessage();
	ret = packet->decodepayload(msg);
	if (packet->event_type == SrcPCUCSetBufferLength) {
		printf("ignored. set buffer length to %d\n", packet->extra_data);
	}
	return ret;
}

int rtmponmetapacket::decodepayload(rtmpcommonmessage* msg)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);

	if ((ret = srs_amf0_read_string(&stream, name)) != 0) {
		printf("decode metadata name failed. ret=%d", ret);
		return ret;
	}

	if (name == SRS_CONSTS_RTMP_SET_DATAFRAME) {
		if ((ret = srs_amf0_read_string(&stream, name)) != 0) {
			printf("decode metadata name failed. ret=%d", ret);
			return ret;
		}
	}

	printf("decode metadata name success. name=%s", name.c_str());

	Amf0Any* any = NULL;
	if ((ret = srs_amf0_read_any(&stream, &any)) != 0) {
		printf("decode metadata metadata failed. ret=%d", ret);
		return ret;
	}

	if (any->is_object()) {
		command_object = any->to_object();
		printf("decode metadata object success");
		return ret;
	}

	return ret;

}

int rtmponmetapacket::encode_packet(streamworker* stream)
{
	int ret = 0;

	if ((ret = srs_amf0_write_string(stream, name)) != 0) {
		printf("encode name failed. ret=%d", ret);
		return ret;
	}
	printf("encode name success.");

	if ((ret = command_object->write(stream)) != 0) {
		printf("encode command_object failed. ret=%d", ret);
		return ret;
	}
	printf("encode metadata success.");

	printf("encode command_object packet success.");
	return ret;
}

int rtmpprotocol::distinguishpacket(rtmpcommonmessage* msg, int headtype, int streamid)
{
	int ret = 0;
	streamworker stream;
	stream.init(msg->payload, msg->size);
	if (headtype == RTMP_MSG_AMF3CommandMessage)
	{
		//if the type id is Am3 , need skip one byte for decode Amf3 data
		printf("type is AMF3 , need skip one byte\n");
		stream.streamback(1);
		msg->payload++;
		msg->size -= 1;
	}
	string command_name;
	srs_amf0_read_string(&stream, command_name);
	if (command_name == "connect")
	{
		rtmpconnectpacket *packet = new rtmpconnectpacket();
		int ret = packet->decodepayload(msg);
		//get tcurl
		Amf0Any* any;
		packet->command_object->get_prop("tcUrl", &any);
		req.tcUrl = ((Amf0String*)any)->value;
		// get objectencoding
		packet->command_object->get_prop("objectEncoding", &any);
		req.objectEncoding = ((Amf0Number*)any)->value;

		srs_discovery_tc_url(req.tcUrl, req.schema, req.host, req.app, req.port,
				req.param);
		printf("discovery app success. tcUrl=%s, schema=%s, port=%s, app=%s\n",
				req.tcUrl.c_str(), req.schema.c_str(), req.port.c_str(), req.app.c_str());
		delete packet;
		return ret;
	}
	else if (command_name == "releaseStream")
	{
		rtmpreleasestreampacket* packet = new rtmpreleasestreampacket();
		int ret = packet->decodepayload(msg);
		rtmpreleasestreamresponsepacket* res_packet = new rtmpreleasestreamresponsepacket(packet->transaction_id);
		send_and_free_packet(res_packet, 0);
		delete packet;
		delete res_packet;
		return ret;
	}
	else if (command_name == "FCPublish")
	{
		rtmpfcpublishpacket* packet = new rtmpfcpublishpacket();
		int ret = packet->decodepayload(msg);
		rtmpfcpublishrespacket* res_packet = new rtmpfcpublishrespacket(packet->transaction_id);
		send_and_free_packet(res_packet, 0);
		delete packet;
		delete res_packet;
		return ret;
	}
	else if (command_name == "FCUnpublish")
	{
		printf("push stream want stop!!!!!!\n");
		rtmpUnPublishPacket* packet = new rtmpUnPublishPacket();
		int ret = packet->decodepayload(msg);
		delete packet;
		//接受到unpublish的command之后就可以进行fd的关闭操作了
		cycleepoll::delclientconnection(m_fd);
		return ret;
	}
	else if (command_name == "createStream")
	{
		streamworker stream;
		stream.init(msg->payload, msg->size);
		rtmpcreatestreampacket* packet = new rtmpcreatestreampacket();
		int ret = packet->decodepayload(msg);
		rtmpcreatestreamrespacket* res_packet = new rtmpcreatestreamrespacket(packet->transaction_id, 1);    //srs也是指定默认回复createstream用的是streamid = 1
		// default stream id for response the createStream request.
		//#define SRS_DEFAULT_SID                         1
		send_and_free_packet(res_packet, 0);
		delete packet;
		delete res_packet;
		return ret;
	}
	else if (command_name == "closeStream")
	{
		printf("client start close stream\n");
		return ret;
	}else if (command_name == "publish")
	{
		rtmppublishpacket* packet = new rtmppublishpacket();
		int ret = packet->decodepayload(msg);
		rtmppublishrespacket* res_packet = new rtmppublishrespacket();
		res_packet->command_object->set_prop_keyvalue("code", Amf0Any::str("NetStream.Publish.Start"));
		res_packet->command_object->set_prop_keyvalue("description", Amf0Any::str("Started publish stream."));
		send_and_free_packet(res_packet, streamid);
		//记录的rtmp地址需要拼接上播放路径:例子:rtmp://127.0.0.1/live/123, 123就是播放路径
		req.tcUrl += packet->stream_name;

		printf("publish url with stream name is %s\n", req.tcUrl.c_str());
		rtmpOnStatusCallPacket* pkt = new rtmpOnStatusCallPacket();
		pkt->data->set_prop_keyvalue(StatusLevel, Amf0Any::str(StatusLevelStatus));
		pkt->data->set_prop_keyvalue(StatusCode, Amf0Any::str(StatusCodePublishStart));
		pkt->data->set_prop_keyvalue(StatusDescription, Amf0Any::str("Started publishing stream."));
		pkt->data->set_prop_keyvalue(StatusClientId, Amf0Any::str(RTMP_SIG_CLIENT_ID));
		send_and_free_packet(pkt, streamid);

		//After publish , check this connection rtmpprotocol if this connection reqest is already exist
		//if not exist need create a new one source

		streamsource* ss = streamsource::getInstance();
		m_source = ss->findsource(&req);
		if (!m_source)
		{
			m_source = ss->createsource(&req,this);
		}

		delete packet;
		delete res_packet;
		delete pkt;
		return ret;
	}
	else if (command_name == "play")
	{
		rtmpplaypacket* packet = new rtmpplaypacket();
		int ret = packet->decodepayload(msg);
		//the same as publish , 需要进行req的url的拼接工作，用拼接后的url去获取steamsource
		req.tcUrl += packet->stream_name;
		printf("play url with stream name is %s\n", req.tcUrl.c_str());
		//send streambegin event packet
		usercontrolpacket* pkt = new usercontrolpacket();
		pkt->event_type = SrcPCUCStreamBegin;
		pkt->event_data = 0;
		send_and_free_packet(pkt, 0);   //user control message must be streamid 0

		rtmpOnStatusCallPacket* onstatusresetpacket = new rtmpOnStatusCallPacket();
		onstatusresetpacket->data->set_prop_keyvalue(StatusLevel, Amf0Any::str(StatusLevelStatus));
		onstatusresetpacket->data->set_prop_keyvalue(StatusCode, Amf0Any::str(StatusCodeStreamReset));
		onstatusresetpacket->data->set_prop_keyvalue(StatusDescription, Amf0Any::str("Playing and resetting stream."));
		onstatusresetpacket->data->set_prop_keyvalue(StatusDetails, Amf0Any::str("stream"));
		onstatusresetpacket->data->set_prop_keyvalue(StatusClientId, Amf0Any::str(RTMP_SIG_CLIENT_ID));
		send_and_free_packet(onstatusresetpacket, streamid);
		printf("send onStatus(NetStream.Play.Reset) message success.\n");


		rtmpOnStatusCallPacket* onstatuspalypkt = new rtmpOnStatusCallPacket();
		onstatuspalypkt->data->set_prop_keyvalue(StatusLevel, Amf0Any::str(StatusLevelStatus));
		onstatuspalypkt->data->set_prop_keyvalue(StatusCode, Amf0Any::str(StatusCodeStreamStart));
		onstatuspalypkt->data->set_prop_keyvalue(StatusDescription, Amf0Any::str("Started playing stream."));
		onstatuspalypkt->data->set_prop_keyvalue(StatusDetails, Amf0Any::str("stream"));
		onstatuspalypkt->data->set_prop_keyvalue(StatusClientId, Amf0Any::str(RTMP_SIG_CLIENT_ID));
		send_and_free_packet(onstatuspalypkt, streamid);
		printf("send onStatus(NetStream.Play.Start) message success.\n");

		rtmpsampleacesspacket *accesspacket = new rtmpsampleacesspacket();
		accesspacket->audio_sample_access = true;
		accesspacket->video_sample_access = true;
		send_and_free_packet(accesspacket, streamid);
		printf("send |RtmpSampleAccess(false, false) message success.\n");


		rtmponstatusdatapacket* onstatusdatapkt = new rtmponstatusdatapacket();
		onstatusdatapkt->data->set_prop_keyvalue(StatusCode, Amf0Any::str(StatusCodeDataStart));
		send_and_free_packet(onstatusdatapkt, streamid);

		printf("send onStatus(NetStream.Data.Start) message success.\n");

		streamsource* ss = streamsource::getInstance();
		m_source = ss->findsource(&req);
		//对于play的客户端而言如果没有得到streamsource就说明没有推流端存在，不应该给客户端传递任何stream
		if (!m_source)
		{
			printf("not exits any url stream source , should create a new source for client and wait for streaming coming\n");
			return -1;
		}
		
		if(m_source->get_rtmpprotocol())
		{
			int ret = 0;
			m_source->get_rtmpprotocol()->add_play_client(m_fd);
			if (send_and_free_message_to_client(&(m_source->get_MessageData()), m_fd) == -1)
				return -1;
			if (send_and_free_message_to_client(&(m_source->get_Video_Sequence_Head()), m_fd) == -1)
				return -1;
			send_gop_cache(m_source->get_rtmpprotocol()->m_cache);
		}
		
		delete packet;
		delete pkt;
	}
	else
	{
		printf("The command name wrong , command_name is %s\n", command_name.c_str());
		return -1;
	}
	return ret;
}


void rtmpprotocol::send_gop_cache(gopcache& cache)
{
	vector<sharedMessage>::iterator itr = (cache.getCache()).begin();
	for(; itr != (cache.getCache()).end() ; itr++)
	{
		send_and_free_message_to_client(&(*itr),m_fd);
	}
}

void rtmpprotocol::add_play_client(int clientfd)
{
	m_clientVec.push_back(clientfd);
}

int rtmpprotocol::create_c1_chunk(char* chunk, rtmpcommonmessageheader* mh)
{
	char* p = chunk;
	char* pp = NULL;
	*p++ = 0x40 | (mh->chunkid & 0x3f);
	if (mh->timestamp_delta < RTMP_EXTENDED_TIMESTAMP)
	{
		pp = (char*)&mh->timestamp_delta;
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	else
	{
		*p++ = 0xFF;
		*p++ = 0xFF;
		*p++ = 0xFF;
	}
	// message_length, 3bytes, big-endian
	pp = (char*)&mh->payload_length;
	*p++ = pp[2];
	*p++ = pp[1];
	*p++ = pp[0];
	// message_type, 1bytes
	*p++ = mh->message_type;

	if (mh->timestamp_delta >= RTMP_EXTENDED_TIMESTAMP) {
		pp = (char*)&mh->timestamp_delta;
		*p++ = pp[3];
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	return p - chunk;
}

int rtmpprotocol::create_c2_chunk(char* chunk, rtmpcommonmessageheader* mh)
{
	char* p = chunk;
	char* pp = NULL;
	*p++ = 0x80 | (mh->chunkid & 0x3f);
	if (mh->timestamp_delta < RTMP_EXTENDED_TIMESTAMP)
	{
		pp = (char*)&mh->timestamp_delta;
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	else
	{
		*p++ = 0xFF;
		*p++ = 0xFF;
		*p++ = 0xFF;
	}

	if (mh->timestamp_delta >= RTMP_EXTENDED_TIMESTAMP) {
		pp = (char*)&mh->timestamp_delta;
		*p++ = pp[3];
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	return p - chunk;
}

int rtmpprotocol::create_c3_chunk(char* chunk, rtmpcommonmessageheader* mh)
{
	char* p = chunk;
	char* pp = NULL;
	*p++ = 0xc0 | (mh->chunkid & 0x3f);

	if (mh->timestamps >= RTMP_EXTENDED_TIMESTAMP) {
		pp = (char*)&mh->timestamps;
		*p++ = pp[3];
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	return p - chunk;
}

int rtmpprotocol::create_c3_chunk(char* chunk, sharedMessageheader* mh)
{
	char* p = chunk;
	char* pp = NULL;
	*p++ = 0xc0 | (mh->chunkid & 0x3f);

	if (mh->timestamps >= RTMP_EXTENDED_TIMESTAMP) {
		pp = (char*)&mh->timestamps;
		*p++ = pp[3];
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	return p - chunk;
}

int rtmpprotocol::on_amfmessagedata(rtmponmetapacket *packet, rtmpcommonmessage* msg, rtmpcommonmessageheader header, int fmt, int amftype)
{
	int ret = 0;
	sharedMessage *message = new sharedMessage();
	if (!message)
	{
		printf("create AMF media data for shared error\n");
		return -1;
	}

	packet->command_object->set_prop_keyvalue("server", Amf0Any::str(RTMP_SIG_SRS_SERVER));
	packet->command_object->set_prop_keyvalue("srs_primary", Amf0Any::str(RTMP_SIG_SRS_PRIMARY));
	packet->command_object->set_prop_keyvalue("srs_authors", Amf0Any::str(RTMP_SIG_SRS_AUTHROS));

	packet->command_object->set_prop_keyvalue("server_version", Amf0Any::str(RTMP_SIG_SRS_VERSION));

	int size = 0;
	char* payload = NULL;
	if ((ret = packet->encode(size, &payload)) != 0)
	{
		printf("Encode server meta data for client play error\n");
		return ret;
	}

	message->messagepalyload = payload;
	message->length = size;
	message->header.timestamps = header.timestamps;
	message->header.chunkid = header.chunkid;
	message->header.message_type = header.message_type;
	message->header.stream_id = header.stream_id;
	message->header.timestamp_delta = header.timestamp_delta;
	message->header.payload_length = message->length;
	m_source->push_msg(message);
	m_source->record_MessageData(*message);
	printf("receive video msg data length %d\n", message->length);
	//printf("receive video msg data PTS is %d,insert video msg data PTS is %d\n",header.timestamps,message->header.timestamps);

	return ret;
}

int rtmpprotocol::on_audiodata(rtmpcommonmessage* msg, rtmpcommonmessageheader header, int fmt)
{
	int ret = 0;
	sharedMessage *message = new sharedMessage();
	if (!message)
	{
		printf("create audio data for shared error\n");
		return -1;
	}
	message->messagepalyload = msg->payload;
	message->length = msg->size;
	message->header.timestamp_delta = header.timestamp_delta;
	message->header.payload_length = header.payload_length;
	message->header.message_type = header.message_type;
	message->header.stream_id = header.stream_id;
	message->header.timestamps = header.timestamps;
	message->header.chunkid = header.chunkid;

	m_source->push_msg(message);
	if(!m_clientVec.empty())
	{
		//when audio has arrived need check cuurent stream protocol if has client connected
		//if has client need send audio data to client
		list<int>::iterator itr = m_clientVec.begin();
		for(;itr != m_clientVec.end(); itr++)
		{
			if (send_and_free_message_to_client(message, *itr) == -1)
				return -1;
		}
	}

	printf("receive audio msg data length %d\n", message->length);

	return ret;
}

int rtmpprotocol::on_videodata(rtmpcommonmessage* msg, rtmpcommonmessageheader header, int fmt)
{
	int ret = 0;
	sharedMessage *message = new sharedMessage();
	if (!message)
	{
		printf("create audio data for shared error\n");
		return -1;
	}
	message->messagepalyload = msg->payload;
	message->length = msg->size;
	message->messagepalyload = msg->payload;
	message->length = msg->size;
	message->header.timestamp_delta = header.timestamp_delta;
	message->header.payload_length = header.payload_length;
	message->header.message_type = header.message_type;
	message->header.stream_id = header.stream_id;
	message->header.timestamps = header.timestamps;
	message->header.chunkid = header.chunkid;
	m_source->push_msg(message);

	m_tsmuxer->on_video(msg->payload, msg->size);

	if(flvcodec::getInstance()->video_is_sequence_header(message->messagepalyload,message->length))
	{
		m_source->record_videoSequenceHead(*message);	
	}
	m_cache.add_gop_cache(*message);
	if(!m_clientVec.empty())
	{
		//when video arrive need check cuurent stream protocol if has client connected
		//if has client need send video data to client
		list<int>::iterator itr = m_clientVec.begin();
		for(;itr != m_clientVec.end(); itr++)
		{
			if (send_and_free_message_to_client(message, *itr) == -1)
				return -1;
		}
	}
	//m_source->pop_msg();
	printf("receive video msg data length %d\n", message->length);
	//printf("receive video msg data PTS is %d,insert video msg data PTS is %d\n",header.timestamps,message->header.timestamps);

	return ret;
}


int rtmpprotocol::recv_message(rtmpcommonmessage** msg)
{
	int ret = 0;
	while (1)
	{
		rtmpcommonmessageheader header;
		int chunkfmt;
		ret = recv_interlaced_message(msg, header, chunkfmt);
		if (ret == -1)
			return -1;
		switch (header.message_type)
		{
		case RTMP_MSG_SetChunkSize:
		{
			rtmpsetchunksizepacket *packet = new rtmpsetchunksizepacket();
			packet->decodepayload(*msg);
			default_chunksize = packet->setchunksize;
			delete *msg;
			*msg = NULL;
			delete packet;
			packet = NULL;
			return ret;
		}break;
		case RTMP_MSG_WindowAcknowledgementSize:
		{
			return ret;
		}break;
		case RTMP_MSG_AMF3CommandMessage:
		{
			int ret = distinguishpacket(*msg, RTMP_MSG_AMF3CommandMessage, header.stream_id);
			((*msg)->payload)--;
			delete *msg;
			*msg = NULL;
			return ret;
		}break;
		case RTMP_MSG_AMF0CommandMessage:
		{
			int ret = distinguishpacket(*msg, RTMP_MSG_AMF0CommandMessage, header.stream_id);
			delete *msg;
			*msg = NULL;
			return ret;
		}break;
		case RTMP_MSG_AMF0DataMessage:
		{
			//
			rtmponmetapacket *packet = new rtmponmetapacket();
			packet->decodepayload(*msg);
			//encode meta packet data and put in the queue for client play
			int ret = on_amfmessagedata(packet, *msg, header, chunkfmt, RTMP_MSG_AMF0CommandMessage);
			printf("Amf0 Data Message  is coming\n ");
		}break;
		case RTMP_MSG_AMF3DataMessage:
		{
			//if it is data message need use the already exist source
			//current not concer about this type id
			return 0;
		}break;
		case RTMP_MSG_AudioMessage:
		{
			//the same as Data message
			int ret = on_audiodata(*msg, header, chunkfmt);
			printf("get audio message\n");
			delete *msg;
			*msg = NULL;
			return ret;
		}break;
		case RTMP_MSG_VideoMessage:
		{
			//the same as Data message
			int ret = on_videodata(*msg, header, chunkfmt);
#if 0
			FILE *file = NULL;
			file = fopen("video.h264","ab");
			if(!file)
				return -1;
			fwrite((*msg)->payload,1,(*msg)->size,file);
			fclose(file);
#endif
			delete *msg;
			*msg = NULL;
			printf("get video message\n");
			return ret;
		}break;
		case RTMP_MSG_UserControlMessage:
		{
			//user control message
			int ret = distinguishusercontrolmessage(*msg);
			delete *msg;
			*msg = NULL;
			return ret;
		}break;
		default:
		{
			printf("type is %d , not need handle it\n", header.message_type);
			return ret;
		}break;
		}
	}
	return ret;
}

int rtmpprotocol::recv_interlaced_message(rtmpcommonmessage** msg, rtmpcommonmessageheader& header, int& chunkfmt)
{
	int ret = 0;
	char fmt = 0;
	int cid = 0;

	if ((ret = readbasicheader(fmt, cid)) != 0)
	{
		//printf("read basic header failed. ret = %d\n", ret);
		return -1;
	}
	chunkfmt = fmt;
	printf("read basic header success. fmt = %d, cid = %d\n", fmt, cid);
	chunkstream* chunk = NULL;
	if (m_chunkstreammap.find(cid) != m_chunkstreammap.end())
	{
		chunk = (m_chunkstreammap.find(cid))->second;
	}
	else
	{
		chunk = new chunkstream(cid);
		m_chunkstreammap.insert(make_pair<int, chunkstream*>(cid, chunk));
	}
	if (readmessageheader(chunk, fmt) == -1)
		return -1;
	if (readmessagepayload(chunk, fmt) == -1)
		return -1;
	if (chunk->header.payload_length == chunk->msg->size)
	{
		//got entire payload from chunk message
		*msg = chunk->msg;
		chunk->msg = NULL;
	}
	else
	{
		*msg = chunk->msg;
	}
	header = chunk->header;
	return ret;

}

int rtmpprotocol::readmessageheader(chunkstream* chunk, char fmt)
{
	int ret = 0;

	bool is_first_chunk_of_msg = !chunk->msg;

	if (chunk->chunk_count == 0 && fmt != 0)
	{
		if (chunk->cid == RTMP_CID_PROTOCOLCONTROL && fmt == RTMP_FMT_TYPE1) {
			printf("accept cid=2, fmt=1 to make librtmp happy.\n");
		}
		else {
			// must be a RTMP protocol level error.
			ret = -1;
			printf("chunk stream is fresh, fmt must be %d, actual is %d. cid=%d, ret=%d\n",
				RTMP_FMT_TYPE0, fmt, chunk->cid, ret);
			return ret;
		}
	}
	//TODO: if fmt is 0 , it shouldn't has chunk msg data , it shuuld be a new stream chunk
	if (chunk->msg && fmt == RTMP_FMT_TYPE0) {
		ret = -1;
		printf("chunk stream exists, "
			"fmt must not be %d, actual is %d. ret=%d\n", RTMP_FMT_TYPE0, fmt, ret);
		return ret;
	}

	// create msg when new chunk stream start
	if (!chunk->msg) {
		chunk->msg = new rtmpcommonmessage();
		printf("create message for new chunk, fmt=%d, cid=%d\n", fmt, chunk->cid);
	}

	static char mh_sizes[] = { 11, 7, 3, 0 };
	int mh_size = mh_sizes[(int)fmt];
	printf("calc chunk message header size. fmt=%d, mh_size=%d\n", fmt, mh_size);
	if (fmt <= RTMP_FMT_TYPE2)
	{
		if (mh_size > 0)
		{
			char* p = new char[mh_size];
			int ret = ioutility::ioread(m_fd, p, mh_size);
			if (ret != mh_size)
				return -1;
			if (fmt <= 2)
			{
				char* pp = (char*)&chunk->header.timestamp_delta;
				pp[2] = *p++;
				pp[1] = *p++;
				pp[0] = *p++;
				pp[3] = 0;
				printf("get chunk timestamps is %d\n", chunk->header.timestamp_delta);

				chunk->extended_timestamp = (chunk->header.timestamp_delta >= RTMP_EXTENDED_TIMESTAMP);
				if (!chunk->extended_timestamp)
					chunk->header.timestamps += chunk->header.timestamp_delta;
				if (fmt <= 1) {
					int payload_length = 0;
					pp = (char*)&payload_length;
					pp[2] = *p++;
					pp[1] = *p++;
					pp[0] = *p++;
					pp[3] = 0;
					chunk->header.payload_length = payload_length;
					chunk->header.message_type = *p++;

					if (fmt == 0)
					{
						pp = (char*)&chunk->header.stream_id;
						pp[0] = *p++;
						pp[1] = *p++;
						pp[2] = *p++;
						pp[3] = *p++;
						printf("header read completed. fmt=%d, mh_size=%d, ext_time=%d,payload=%d, type=%d, sid=%d\n",
							fmt, mh_size, chunk->extended_timestamp, chunk->header.payload_length,
							chunk->header.message_type, chunk->header.stream_id);
					}
					else
					{
						printf("header read completed. fmt= %d, mh_size=%d, ext_time=%d,payload=%d,type=%d,sid=%d\n",
							fmt, mh_size, chunk->extended_timestamp, chunk->header.payload_length,
							chunk->header.message_type, chunk->header.stream_id);
					}
				}
				else
				{
					printf("header read completed. fmt=%d, mh_size=%d, ext_time=%d\n",
						fmt, mh_size, chunk->extended_timestamp);
				}
			}
			else
			{
				printf("header read completed. fmt=%d, size=%d, ext_time=%d\n",
					fmt, mh_size, chunk->extended_timestamp);
			}
		}
	}
	else
	{
		//fmt is 3
		if (is_first_chunk_of_msg && !chunk->extended_timestamp) {
			chunk->header.timestamps += chunk->header.timestamp_delta;
		}
		printf("header read completed. fmt=%d, size=%d, ext_time=%d\n",
			fmt, mh_size, chunk->extended_timestamp);
	}
	//message header 瑙ｆ瀽鎴愬姛
	if (chunk->extended_timestamp)
	{
		mh_size += 4;
		printf("read header ext time. fmt = %d,ext_time=%d,mh_size=%d\n", fmt, chunk->extended_timestamp, mh_size);
		char *p = new char[4];
		int ret = ioutility::ioread(m_fd, p, 4);
		if (ret != 4)
			return -1;
		int timestamps = 0x00;
		char* pp = (char*)&timestamps;
		pp[3] = *p++;
		pp[2] = *p++;
		pp[1] = *p++;
		pp[0] = *p++;

		timestamps &= 0x7fffffff;
		if (fmt == 0)
		{
			chunk->header.timestamps = timestamps;
			chunk->header.timestamps &= 0x7fffffff;
		}
		else
		{
			chunk->header.timestamps += timestamps;
		}
	}
	chunk->chunk_count++;  //if chunk_count != 0 , it means can recieve fmt 1,2,3

	return ret;
}

int rtmpprotocol::readmessagepayload(chunkstream* chunk, char fmt)
{

	if (chunk->header.payload_length == 0)
	{
		return -1;
	}
	char* p = new char[chunk->header.payload_length];
	if (!p)
		return -1;
	char* copyp = p;
	int hasread = 0;
	int remaindata = chunk->header.payload_length;

	while (1)
	{
		int shouldreadsize = 0;
		if (remaindata < default_chunksize)
		{
			shouldreadsize = remaindata;
		}
		else
		{
			shouldreadsize = default_chunksize;
		}
		int ret = 0;
		while (1)
		{

			ret = ioutility::ioread(m_fd, copyp + hasread, shouldreadsize);
			shouldreadsize -= ret;

			hasread += ret;
			remaindata -= ret;
			printf("read data size is %d, remain data size is %d\n", ret, remaindata);
			if (shouldreadsize == 0)
			{
				break;
			}

		}

		if (hasread == chunk->header.payload_length)
		{
			printf("read message playload success, size is %d\n", hasread);
			break;
		}
		else if (hasread > chunk->header.payload_length)
		{
			printf("get message playload error , is bigger than payload_length, size is %d\n", hasread);
			return -1;
		}
		else
		{
			char fmt = 0;
			int cid = 0;
			if ((ret = readbasicheader(fmt, cid)) != 0)
			{
				//printf("read basic header failed. ret = %d\n", ret);
			}
			if (fmt == 0x03)
			{
				continue;
			}
			//Type 1 chunk headers are 7 bytes long.  The message stream ID is not   included; 
			//this chunk takes the same stream ID as the preceding chunk.   
			//Streams with variable-sized messages (for example, many video   formats) SHOULD use this 
			//format for the first chunk of each new message after the first.
			else if (fmt == 0x01)
			{
				if (chunk->header.message_type != RTMP_MSG_VideoMessage)
				{
					printf("After fmt0 message is a fmt1 message , This should only be video type , but it is not a video \
						   						, please check it\n");
				}
				chunkstream partialchunk(cid);
				readmessageheader(&partialchunk, fmt);

			}
		}

	}
	if (hasread != chunk->header.payload_length)
		return -1;
	chunk->msg->payload = p;
	chunk->msg->size = chunk->header.payload_length;
	p = NULL;
	return 0;
}

/*
0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+
|fmt|   cs id   |
+-+-+-+-+-+-+-+-+
Chunk basic header 1
Chunk stream IDs 2-63 can be encoded in the 1-byte version of this  field

0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|fmt|     0     |   cs id - 64  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Chunk basic header 2
Chunk stream IDs 64-65599 can be encoded in the 3-byte version of
this field.  ID is computed as ((the third byte)*256 + (the second   byte) + 64).

0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|fmt|     1     |        cs id - 64             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Chunk basic header 3
Chunk stream IDs 64-65599 can be encoded in the 3-byte version of
this field.  ID is computed as ((the third byte)*256 + (the second   byte) + 64)
*/
int rtmpprotocol::readbasicheader(char& fmt, int& cid)
{
	int ret = ioutility::ioread(m_fd, (char*)&fmt, 1);
	if (ret == 0)
	{
		//printf("read fmt error, the socket fd has empty,read size is %d\n", ret);
		return -1;
	}
	cid = fmt & 0x3f;
	fmt = (fmt >> 6) & 0x03;
	//2-63 , 1byte chunk basic head
	if (cid > 1)
	{
		printf("basic header parsed, fmt = %d,cid = %d\n", fmt, cid);
		return 0;
	}

	if (cid == 0) //64-319 , 2B chunk head
	{
		char onebyte;
		ioutility::ioread(m_fd, &onebyte, 1);
		cid = 64;
		cid += onebyte;
		if (onebyte == 48)
		{
			printf("stop\n");
		}
		printf("basic header parsed,one bytes fmt = %d,cid = %d ,byte=%x\n", fmt, cid, onebyte);
	}
	else if (cid == 1)
	{
		char twobytes[2];
		ioutility::ioread(m_fd, twobytes, 2);
		cid = 64;
		cid += twobytes[0];
		cid += twobytes[1] * 256;
		printf("basic header parsed,two bytes fmt = %d,cid = %d\n", fmt, cid);
	}
	else
	{
		printf("invalid path, impossible basic header.");
		return -1;
	}

	return 0;
}

int rtmpprotocol::send_and_free_packet(rtmpcommonpacket* packet, int stream_id)
{
	int ret = 0;

	int size = 0;
	char* payload = NULL;
	if ((ret = packet->encode(size, &payload)) != 0) {
		printf("encode RTMP packet to bytes oriented RTMP message failed. ret=%d\n", ret);
		return ret;
	}

	// encode packet to payload and size.
	if (size <= 0 || payload == NULL) {
		printf("packet is empty, ignore empty message.\n");
		return ret;
	}

	rtmpcommonmessageheader header;
	header.payload_length = size;
	header.message_type = packet->get_message_type();
	header.stream_id = stream_id;
	header.chunkid = packet->get_prefer_chunkid();

	do_packet_send(&header, payload, size);
	delete[] payload;
	return 0;
}

int rtmpprotocol::do_packet_send(rtmpcommonmessageheader* mh, char* payload, int size)
{
	int ret = 0;

	// we directly send out the packet,
	// use very simple algorithm, not very fast,
	// but it's ok.
	char* p = payload;
	char* end = p + size;
	char c0c3[SRS_CONSTS_RTMP_MAX_FMT0_HEADER_SIZE];

	while (p != end)
	{
		int nbh = 0;
		if (p == payload) {
			//create fmt 0 chunk
			int len = create_c0_chunk(c0c3, mh);
			char* chunk = new char[len + size];
			memcpy(chunk, c0c3, len);
			memcpy(chunk + len, payload, size);
			printf("send chunk packet\n");
			int num = ioutility::iowrite(m_fd, chunk, len + size);
			if (num != (len + size))
			{
				printf("send packet error\n");
				return -1;
			}
			p += size;
		}
		else {

		}
	}
}

int rtmpprotocol::create_c0_chunk(char* chunk, rtmpcommonmessageheader* mh)
{
	char* p = chunk;
	char* pp = NULL;
	*p++ = 0x00 | (mh->chunkid & 0x3f);
	if (mh->timestamps < RTMP_EXTENDED_TIMESTAMP)
	{
		pp = (char*)&mh->timestamps;
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	else
	{
		*p++ = 0xFF;
		*p++ = 0xFF;
		*p++ = 0xFF;
	}
	// message_length, 3bytes, big-endian
	pp = (char*)&mh->payload_length;
	*p++ = pp[2];
	*p++ = pp[1];
	*p++ = pp[0];
	// message_type, 1bytes
	*p++ = mh->message_type;
	// stream_id, 4bytes, little-endian
	pp = (char*)&mh->stream_id;
	*p++ = pp[0];
	*p++ = pp[1];
	*p++ = pp[2];
	*p++ = pp[3];
	if (mh->timestamps >= RTMP_EXTENDED_TIMESTAMP) {
		pp = (char*)&mh->timestamps;
		*p++ = pp[3];
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	return p - chunk;
}

int rtmpprotocol::create_c0_chunk(char* chunk, sharedMessageheader* mh)
{
	char* p = chunk;
	char* pp = NULL;
	*p++ = 0x00 | (mh->chunkid & 0x3f);
	if (mh->timestamps < RTMP_EXTENDED_TIMESTAMP)
	{
		pp = (char*)&mh->timestamps;
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	else
	{
		*p++ = 0xFF;
		*p++ = 0xFF;
		*p++ = 0xFF;
	}
	// message_length, 3bytes, big-endian
	pp = (char*)&mh->payload_length;
	*p++ = pp[2];
	*p++ = pp[1];
	*p++ = pp[0];
	// message_type, 1bytes
	*p++ = mh->message_type;
	// stream_id, 4bytes, little-endian
	pp = (char*)&mh->stream_id;
	*p++ = pp[0];
	*p++ = pp[1];
	*p++ = pp[2];
	*p++ = pp[3];
	if (mh->timestamps >= RTMP_EXTENDED_TIMESTAMP) {
		pp = (char*)&mh->timestamps;
		*p++ = pp[3];
		*p++ = pp[2];
		*p++ = pp[1];
		*p++ = pp[0];
	}
	return p - chunk;
}

int rtmpprotocol::response_connect_app()
{
	int ret = 0;
	rtmpconnectresponsepacket *pkt = new rtmpconnectresponsepacket();
	pkt->m_prop->set_prop_keyvalue("fmsVer", Amf0Any::str("FMS/"RTMP_SIG_FMS_VER));
	pkt->m_prop->set_prop_keyvalue("capabilities", Amf0Any::number(127));
	pkt->m_prop->set_prop_keyvalue("mode", Amf0Any::number(1));

	pkt->m_info->set_prop_keyvalue(StatusLevel, Amf0Any::str(StatusLevelStatus));
	pkt->m_info->set_prop_keyvalue(StatusCode, Amf0Any::str(StatusCodeConnectSuccess));
	pkt->m_info->set_prop_keyvalue(StatusDescription, Amf0Any::str("Connection succeeded"));
	pkt->m_info->set_prop_keyvalue("objectEncoding", Amf0Any::number(req.objectEncoding));

	Amf0EcmaArray* data = new Amf0EcmaArray();
	pkt->m_info->set_prop_keyvalue("data", data);
	data->set_prop("version", Amf0Any::str(RTMP_SIG_FMS_VER));
	data->set_prop("srs_sig", Amf0Any::str(RTMP_SIG_SRS_KEY));
	data->set_prop("srs_server", Amf0Any::str(RTMP_SIG_SRS_SERVER));
	data->set_prop("srs_license", Amf0Any::str(RTMP_SIG_SRS_LICENSE));
	data->set_prop("srs_role", Amf0Any::str(RTMP_SIG_SRS_ROLE));
	data->set_prop("srs_url", Amf0Any::str(RTMP_SIG_SRS_URL));
	data->set_prop("srs_version", Amf0Any::str(RTMP_SIG_SRS_VERSION));
	data->set_prop("srs_site", Amf0Any::str(RTMP_SIG_SRS_WEB));
	data->set_prop("srs_email", Amf0Any::str(RTMP_SIG_SRS_EMAIL));
	data->set_prop("srs_copyright", Amf0Any::str(RTMP_SIG_SRS_COPYRIGHT));
	data->set_prop("srs_primary", Amf0Any::str(RTMP_SIG_SRS_PRIMARY));
	data->set_prop("srs_authors", Amf0Any::str(RTMP_SIG_SRS_AUTHROS));
	data->set_prop("srs_server_ip", Amf0Any::str("192.168.1.212"));
	//data->set_prop("srs_pid", SrsAmf0Any::number(getpid()));
	//data->set_prop("srs_id", SrsAmf0Any::number(_srs_context->get_id()));
	if ((ret = send_and_free_packet(pkt, 0)) != 0) {
		printf("send connect app response message failed. ret=%d\n", ret);
		return ret;
	}
	printf("send connect app response message success.\n");
	return ret;
}

void rtmpprotocol::remove_play_client(int fd)
{
	printf("client vec size is %d\n", m_clientVec.size());
	list<int>::iterator itr = m_clientVec.begin();
	for (; itr != m_clientVec.end(); itr++)
	{
		if (*itr == fd)
		{
			m_clientVec.erase(itr);
			//after erase need return , because the m_clientvec itr has changed and can't use anymore
			return;
		}
	}
}

int rtmpprotocol::send_and_free_message_to_client(sharedMessage* msg, int clientfd)
{
	char* p = msg->messagepalyload;
	char* pend = msg->messagepalyload + msg->length;
	char c0c3[SRS_CONSTS_RTMP_MAX_FMT0_HEADER_SIZE];
	int hn = 0;
	while (p < pend)
	{
		memset(c0c3, 0, SRS_CONSTS_RTMP_MAX_FMT0_HEADER_SIZE);
		if (p == msg->messagepalyload)
		{
			//create chunk head 0
			hn = create_c0_chunk(c0c3, &msg->header);
			int totalsize = msg->length + hn;
			char* sendbuf = NULL;
			if (totalsize > 4096)
			{
				sendbuf = new char[4096 + hn];    //这里用chunkheader的大小加上协商的chunksize的大小愿意是ffmpeg在
				//解析的时候 当大于chunksize的包会按照chunksize读取数据，这样就导致实体发送的数据没有chunksize的大小
				totalsize = 4096 + hn;
			}
			else
			{
				sendbuf = new char[totalsize];
			}
			memcpy(sendbuf, c0c3, hn);
			int buflength = totalsize - hn;
			memcpy(sendbuf + hn, p, buflength);
			int num = ioutility::iowrite(clientfd, sendbuf, totalsize);
			if (num == CLIENT_CLOSED)
			{
				remove_play_client(clientfd);
				return -1;
			}
			if (num < hn)
			{
				printf("has send client data not biger than chunk header 0   %d\n",__LINE__);
			}
			p += (num - hn);
			delete[] sendbuf;
		}
		else
		{
			//create chunk head 3
			hn = create_c3_chunk(c0c3, &msg->header);
			int remainsize = pend - p;
			char* sendbuf = NULL;
			if (remainsize > 4096)
			{
				sendbuf = new char[4096 + hn];
				remainsize = 4096 + hn;
			}
			else
			{
				sendbuf = new char[remainsize + hn];
				remainsize = remainsize + hn;
			}
			memcpy(sendbuf, c0c3, hn);
			int buflength = remainsize - hn;
			memcpy(sendbuf + hn, p, buflength);
			int num = ioutility::iowrite(clientfd, sendbuf, remainsize);
			if (num == CLIENT_CLOSED)
			{
				remove_play_client(clientfd);
				return -1;
			}
			if (num < hn)
			{
				printf("has send client data not biger than chunk header 0   %d\n",__LINE__);
			}
			p += (num - hn);
			delete[] sendbuf;
		}
	}
	return 0;

}


int rtmpprotocol::send_and_free_message_to_client(sharedMessage* msg)
{

	char* p = msg->messagepalyload;
	char* pend = msg->messagepalyload + msg->length;
	char c0c3[SRS_CONSTS_RTMP_MAX_FMT0_HEADER_SIZE];
	int hn = 0;
	while (p < pend)
	{
		memset(c0c3, 0, SRS_CONSTS_RTMP_MAX_FMT0_HEADER_SIZE);
		if (p == msg->messagepalyload)
		{
			//create chunk head 0
			hn = create_c0_chunk(c0c3, &msg->header);
			int totalsize = msg->length + hn;
			char* sendbuf = NULL;
			if (totalsize > 4096)
			{
				sendbuf = new char[4096 + hn];    //这里用chunkheader的大小加上协商的chunksize的大小愿意是ffmpeg在
				//解析的时候 当大于chunksize的包会按照chunksize读取数据，这样就导致实体发送的数据没有chunksize的大小
				totalsize = 4096 + hn;
			}
			else
			{
				sendbuf = new char[totalsize];
			}
			memcpy(sendbuf, c0c3, hn);
			int buflength = totalsize - hn;
			memcpy(sendbuf + hn, p, buflength);
			int num = ioutility::iowrite(m_fd, sendbuf, totalsize);
			if (num == CLIENT_CLOSED)
			{
				remove_play_client(m_fd);
				return -1;
			}
			if (num < hn)
			{
				printf("has send client data not biger than chunk header 0   %d\n",__LINE__);
			}
			p += (num - hn);
			delete[] sendbuf;
		}
		else
		{
			//create chunk head 3
			hn = create_c3_chunk(c0c3, &msg->header);
			int remainsize = pend - p;
			char* sendbuf = NULL;
			if (remainsize > 4096)
			{
				sendbuf = new char[4096 + hn];
				remainsize = 4096 + hn;
			}
			else
			{
				sendbuf = new char[remainsize + hn];
				remainsize = remainsize + hn;
			}
			memcpy(sendbuf, c0c3, hn);
			int buflength = remainsize - hn;
			memcpy(sendbuf + hn, p, buflength);
			int num = ioutility::iowrite(m_fd, sendbuf, remainsize);
			if (num == CLIENT_CLOSED)
			{
				remove_play_client(m_fd);
				return -1;
			}
			if (num < hn)
			{
				printf("has send client data not biger than chunk header 0   %d\n",__LINE__);
			}
			p += (num - hn);
			delete[] sendbuf;
		}
	}
	return 0;
}

int rtmpprotocol::pull_and_send_stream()
{
	if (!m_source)
		return -1;
	static bool isfirst = true;
	if(isfirst)
	{
		//靠靠靠靠靠靠messagedata靠縞lient靠縞lient靠靠靠?		isfirst = false;
		sharedMessage msg = m_source->get_MessageData();
		if( -1 == send_and_free_message_to_client(&msg))
		{
			printf("A new client need play, and message data send first take error\n");
			return -1;
		}
	}
	sharedMessage* msg = m_source->get_front();
	if (!msg)
		return -1;

	//目前对于拉流没有策略，拉一个出队列一个
	m_source->pop_msg();
	//推送流数据到客户端
	if (-1 == send_and_free_message_to_client(msg))
	{
		return -1;
	}
	return 0;
}

int rtmpprotocol::stream_service_cycle()
{
	rtmpcommonmessage* msg = new rtmpcommonmessage();
	int ret = recv_message(&msg);
	return ret;
}

