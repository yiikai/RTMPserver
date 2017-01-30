#ifndef RTMP_PROTOCOL_H
#define RTMP_PROTOCOL_H

#include <string>
#include "amf0.h"
#include "streamworker.h"
#include "requestinfo.h"
#include "streamsource.h"
#include "rtmpcomplexhandshake.h"
#include "rtmpcore.h"
#include "gopcache.h"
#include <vector>
#include <list>
using namespace std;

#define RTMP_MSG_SetChunkSize                   0x01
#define RTMP_MSG_AbortMessage                   0x02
#define RTMP_MSG_Acknowledgement                0x03
#define RTMP_MSG_UserControlMessage             0x04
#define RTMP_MSG_WindowAcknowledgementSize      0x05
#define RTMP_MSG_SetPeerBandwidth               0x06
#define RTMP_MSG_EdgeAndOriginServerCommand     0x07




/****************************************************************************
*****************************************************************************
****************************************************************************/
/**
* 6.1.2. Chunk Message Header
* There are four different formats for the chunk message header,
* selected by the "fmt" field in the chunk basic header.
*/
// 6.1.2.1. Type 0
// Chunks of Type 0 are 11 bytes long. This type MUST be used at the
// start of a chunk stream, and whenever the stream timestamp goes
// backward (e.g., because of a backward seek).
#define RTMP_FMT_TYPE0                          0
// 6.1.2.2. Type 1
// Chunks of Type 1 are 7 bytes long. The message stream ID is not
// included; this chunk takes the same stream ID as the preceding chunk.
// Streams with variable-sized messages (for example, many video
// formats) SHOULD use this format for the first chunk of each new
// message after the first.
#define RTMP_FMT_TYPE1                          1
// 6.1.2.3. Type 2
// Chunks of Type 2 are 3 bytes long. Neither the stream ID nor the
// message length is included; this chunk has the same stream ID and
// message length as the preceding chunk. Streams with constant-sized
// messages (for example, some audio and data formats) SHOULD use this
// format for the first chunk of each message after the first.
#define RTMP_FMT_TYPE2                          2
// 6.1.2.4. Type 3
// Chunks of Type 3 have no header. Stream ID, message length and
// timestamp delta are not present; chunks of this type take values from
// the preceding chunk. When a single message is split into chunks, all
// chunks of a message except the first one, SHOULD use this type. Refer
// to example 2 in section 6.2.2. Stream consisting of messages of
// exactly the same size, stream ID and spacing in time SHOULD use this
// type for all chunks after chunk of Type 2. Refer to example 1 in
// section 6.2.1. If the delta between the first message and the second
// message is same as the time stamp of first message, then chunk of
// type 3 would immediately follow the chunk of type 0 as there is no
// need for a chunk of type 2 to register the delta. If Type 3 chunk
// follows a Type 0 chunk, then timestamp delta for this Type 3 chunk is
// the same as the timestamp of Type 0 chunk.
#define RTMP_FMT_TYPE3                          3


#define RTMP_MSG_AMF3CommandMessage             17 // 0x11
#define RTMP_MSG_AMF0CommandMessage             20 // 0x14
#define RTMP_MSG_AMF0DataMessage                18 // 0x12
#define RTMP_MSG_AMF3DataMessage                15 // 0x0F
#define RTMP_MSG_AMF3SharedObject               16 // 0x10
#define RTMP_MSG_AMF0SharedObject               19 // 0x13
#define RTMP_MSG_AudioMessage                   8 // 0x08
#define RTMP_MSG_VideoMessage                   9 // 0x09
#define RTMP_MSG_AggregateMessage               22 // 0x16


#define RTMP_AMF0_COMMAND_CONNECT               "connect"
#define RTMP_AMF0_COMMAND_CREATE_STREAM         "createStream"
#define RTMP_AMF0_COMMAND_CLOSE_STREAM          "closeStream"
#define RTMP_AMF0_COMMAND_PLAY                  "play"
#define RTMP_AMF0_COMMAND_PAUSE                 "pause"
#define RTMP_AMF0_COMMAND_ON_BW_DONE            "onBWDone"
#define RTMP_AMF0_COMMAND_ON_STATUS             "onStatus"
#define RTMP_AMF0_COMMAND_RESULT                "_result"
#define RTMP_AMF0_COMMAND_ERROR                 "_error"
#define RTMP_AMF0_COMMAND_RELEASE_STREAM        "releaseStream"
#define RTMP_AMF0_COMMAND_FC_PUBLISH            "FCPublish"
#define RTMP_AMF0_COMMAND_UNPUBLISH             "FCUnpublish"
#define RTMP_AMF0_COMMAND_PUBLISH               "publish"
#define RTMP_AMF0_COMMAND_DELETE_STREAM         "deleteStream"
#define RTMP_AMF0_DATA_SAMPLE_ACCESS            "|RtmpSampleAccess"

/**
 * the signature for packets to client.
 */
#define RTMP_SIG_FMS_VER                        "3,5,3,888"
#define RTMP_SIG_AMF0_VER                       0
#define RTMP_SIG_CLIENT_ID                      "ASAICiss"

/**
 * onStatus consts.
 */
#define StatusLevel                             "level"
#define StatusCode                              "code"
#define StatusDescription                       "description"
#define StatusDetails                           "details"
#define StatusClientId                          "clientid"
// status value
#define StatusLevelStatus                       "status"
// status error
#define StatusLevelError                        "error"
// code value
#define StatusCodeConnectSuccess                "NetConnection.Connect.Success"
#define StatusCodeConnectRejected               "NetConnection.Connect.Rejected"
#define StatusCodeStreamReset                   "NetStream.Play.Reset"
#define StatusCodeStreamStart                   "NetStream.Play.Start"
#define StatusCodeStreamPause                   "NetStream.Pause.Notify"
#define StatusCodeStreamUnpause                 "NetStream.Unpause.Notify"
#define StatusCodePublishStart                  "NetStream.Publish.Start"
#define StatusCodeDataStart                     "NetStream.Data.Start"
#define StatusCodeUnpublishSuccess              "NetStream.Unpublish.Success"

/****************************************************************************
*****************************************************************************/
#define SRS_CONSTS_RTMP_MAX_FMT0_HEADER_SIZE 16
#define RTMP_EXTENDED_TIMESTAMP                 0xFFFFFF

#define RTMP_CID_PROTOCOLCONTROL                0x02
#define RTMP_CID_NetConnection                 0x03
#define RTMP_CID_NetStream                     0x04
#define RTMP_CID_ONSTATUS                       0x05

// stable major version
#define VERSION_STABLE 1
#define VERSION_STABLE_BRANCH SRS_XSTR(VERSION_STABLE)".0release"

#define SRS_CONSTS_RTMP_SET_DATAFRAME            "@setDataFrame"
#define SRS_CONSTS_RTMP_ON_METADATA              "onMetaData"






// 3.7. User Control message
enum SrcPCUCEventType
{
    // generally, 4bytes event-data
    
    /**
    * The server sends this event to notify the client
    * that a stream has become functional and can be
    * used for communication. By default, this event
    * is sent on ID 0 after the application connect
    * command is successfully received from the
    * client. The event data is 4-byte and represents
    * the stream ID of the stream that became
    * functional.
    */
    SrcPCUCStreamBegin              = 0x00,

    /**
    * The server sends this event to notify the client
    * that the playback of data is over as requested
    * on this stream. No more data is sent without
    * issuing additional commands. The client discards
    * the messages received for the stream. The
    * 4 bytes of event data represent the ID of the
    * stream on which playback has ended.
    */
    SrcPCUCStreamEOF                = 0x01,

    /**
    * The server sends this event to notify the client
    * that there is no more data on the stream. If the
    * server does not detect any message for a time
    * period, it can notify the subscribed clients 
    * that the stream is dry. The 4 bytes of event 
    * data represent the stream ID of the dry stream. 
    */
    SrcPCUCStreamDry                = 0x02,

    /**
    * The client sends this event to inform the server
    * of the buffer size (in milliseconds) that is 
    * used to buffer any data coming over a stream.
    * This event is sent before the server starts  
    * processing the stream. The first 4 bytes of the
    * event data represent the stream ID and the next
    * 4 bytes represent the buffer length, in 
    * milliseconds.
    */
    SrcPCUCSetBufferLength          = 0x03, // 8bytes event-data

    /**
    * The server sends this event to notify the client
    * that the stream is a recorded stream. The
    * 4 bytes event data represent the stream ID of
    * the recorded stream.
    */
    SrcPCUCStreamIsRecorded         = 0x04,

    /**
    * The server sends this event to test whether the
    * client is reachable. Event data is a 4-byte
    * timestamp, representing the local server time
    * when the server dispatched the command. The
    * client responds with kMsgPingResponse on
    * receiving kMsgPingRequest.  
    */
    SrcPCUCPingRequest              = 0x06,

    /**
    * The client sends this event to the server in
    * response to the ping request. The event data is
    * a 4-byte timestamp, which was received with the
    * kMsgPingRequest request.
    */
    SrcPCUCPingResponse             = 0x07,
    
    /**
     * for PCUC size=3, the payload is "00 1A 01",
     * where we think the event is 0x001a, fms defined msg,
     * which has only 1bytes event data.
     */
    SrsPCUCFmsEvent0                = 0x1a,
};

typedef enum RTMPSTATUS
{
	RTMPSTATUS_NOCONNECT = -1,
	RTMPSTATUS_HANDSHAKEC0C1 = 0,
	RTMPSTATUS_HANDSHAKEC2,
	RTMPSTATUS_RTMPCONNECT,
	RTMPSTATUS_STREAMSERVICE,
};

class rtmpcommonmessage;
class rtmpcommonmessageheader;

class rtmpcommonpacket
{
public:
	rtmpcommonpacket(){

	}
	virtual ~rtmpcommonpacket(){

	}
	
	virtual int decodepayload(rtmpcommonmessage* msg) = 0;
	virtual int encode_packet(streamworker* stream) = 0;

	int encode(int& size , char** ppayload);
	virtual int get_message_type() = 0;
	virtual int get_prefer_chunkid() = 0;
protected:
	virtual int get_size() = 0;
};

class setwindowacksizepacket : public rtmpcommonpacket
{
public:
	setwindowacksizepacket();
	~setwindowacksizepacket();
	int decodepayload(rtmpcommonmessage* msg)
	{
		return 0;
	}
	int encode_packet(streamworker* stream);
	int get_message_type();
	int get_prefer_chunkid(){
		return RTMP_CID_PROTOCOLCONTROL;
	}
public:
	int ackowledgement_window_size;
private:
	int get_size();
	
};

class setpeerbandwidthpacket : public rtmpcommonpacket
{
	public:
		setpeerbandwidthpacket();
		~setpeerbandwidthpacket();
		int decodepayload(rtmpcommonmessage* msg)
		{
			return 0;
		}
		int encode_packet(streamworker* stream);
		int get_message_type(){
			return RTMP_MSG_SetPeerBandwidth;
		}
		int get_prefer_chunkid(){
			return RTMP_CID_PROTOCOLCONTROL;
		}
	public:
		int bandwidth;
		char type;
	private:
		int get_size(){
			return 5;
		}
};

class setchunksizepacket : public rtmpcommonpacket
{
public:
	setchunksizepacket():size(-1){}
	~setchunksizepacket(){}
	int decodepayload(rtmpcommonmessage* msg)
	{
		return 0;
	}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_SetChunkSize;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_PROTOCOLCONTROL;
	}
	public:
		int size;
	private:
		int get_size(){
			return 4;
		}
};

class rtmponbwdonepacket : public rtmpcommonpacket
{
public:
	rtmponbwdonepacket():command_name(RTMP_AMF0_COMMAND_ON_BW_DONE),transaction_id(0){}
	~rtmponbwdonepacket(){}

	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
public:
	string command_name;
	double transaction_id;
private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number()
        + Amf0Size::null();
	}
};

class rtmpreleasestreampacket : public rtmpcommonpacket
{
public:
	rtmpreleasestreampacket(){}
	~rtmpreleasestreampacket(){}
	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream){}
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
public:
	string command_name;
	double transaction_id;
private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number();
	}
};

class rtmpreleasestreamresponsepacket:public rtmpcommonpacket
{
	public:
		rtmpreleasestreamresponsepacket(double t_id):command_name("_result"){
			transaction_id = t_id;
		}
		~rtmpreleasestreamresponsepacket(){}
		int decodepayload(rtmpcommonmessage* msg){}
		int encode_packet(streamworker* stream);
		int get_message_type(){
			return RTMP_MSG_AMF0CommandMessage;
		}

		int get_prefer_chunkid(){
			return RTMP_CID_NetConnection;
		}
	public:
		double transaction_id;
		string command_name;
	private:
		int get_size(){
			return Amf0Size::str(command_name) + Amf0Size::number();
		}
};

class rtmpfcpublishrespacket:public rtmpcommonpacket
{
public:
	rtmpfcpublishrespacket(double t_id):command_name("_result"){
		transaction_id = t_id;
	}
	~rtmpfcpublishrespacket(){}
	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
private:
		int get_size(){
			return Amf0Size::str(command_name) + Amf0Size::number()
        		+ Amf0Size::null() + Amf0Size::undefined();
		}
public:
	double transaction_id;
	string command_name;
};

class rtmpfcpublishpacket : public rtmpcommonpacket
{
public:
	rtmpfcpublishpacket(){}
	~rtmpfcpublishpacket(){}

	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream){}
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number()
        		+ Amf0Size::null() + Amf0Size::str(stream_name);
	}
public:
	string command_name;
	string stream_name;
	double transaction_id;
};

class rtmpcreatestreamrespacket : public rtmpcommonpacket
{
public:
	rtmpcreatestreamrespacket(double t_id, double s_id):command_name("_result"){
		transaction_id = t_id;
        stream_id = s_id;
	}
	~rtmpcreatestreamrespacket(){}
	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}

private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number()
        		+ Amf0Size::null() + Amf0Size::number();
	}
public:
	string command_name;
	double transaction_id;
    double stream_id;
};

class rtmppublishpacket : public rtmpcommonpacket
{
public:
	rtmppublishpacket(){}
	~rtmppublishpacket(){}
	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream){}
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetStream;
	}
private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number()
        		+ Amf0Size::null() + Amf0Size::str(stream_name) + Amf0Size::str(app_name);
	}
public:
	string command_name;
	double transaction_id;
	string stream_name;
	string app_name;
};

class rtmppublishrespacket : public rtmpcommonpacket
{
public:
	rtmppublishrespacket(){
		command_object = new amf0object();
	}
	~rtmppublishrespacket(){
		if(command_object)
		{
			delete command_object;
			command_object = NULL;
		}
	}

	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_ONSTATUS;
	}

private:
	int get_size(){
		return Amf0Size::str("onFCPublish") + Amf0Size::number() + Amf0Size::null() +
		         Amf0Size::object(command_object);
	}

public:
	amf0object* command_object;
};

class rtmponmetapacket : public rtmpcommonpacket
{
public:
	rtmponmetapacket()
	{
		name = SRS_CONSTS_RTMP_ON_METADATA;
		command_object = new amf0object();
	}
    ~rtmponmetapacket(){}

	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetStream;
	}
private:
	int get_size(){
		return Amf0Size::str(name) + Amf0Size::object(command_object);
	}
public:
	string name;
	amf0object* command_object;

};
 


class rtmpplaypacket : public rtmpcommonpacket
{
public:
    /**
    * Name of the command. Set to "play".
    */
    std::string command_name;
    /**
    * Transaction ID set to 0.
    */
    double transaction_id;
    /**
    * Command information does not exist. Set to null type.
    * @remark, never be NULL, an AMF0 null instance.
    */
    Amf0Any* command_object; // null
    /**
    * Name of the stream to play.
    * To play video (FLV) files, specify the name of the stream without a file
    *       extension (for example, "sample").
    * To play back MP3 or ID3 tags, you must precede the stream name with mp3:
    *       (for example, "mp3:sample".)
    * To play H.264/AAC files, you must precede the stream name with mp4: and specify the
    *       file extension. For example, to play the file sample.m4v, specify 
    *       "mp4:sample.m4v"
    */
    std::string stream_name;
    /**
    * An optional parameter that specifies the start time in seconds.
    * The default value is -2, which means the subscriber first tries to play the live 
    *       stream specified in the Stream Name field. If a live stream of that name is 
    *       not found, it plays the recorded stream specified in the Stream Name field.
    * If you pass -1 in the Start field, only the live stream specified in the Stream 
    *       Name field is played.
    * If you pass 0 or a positive number in the Start field, a recorded stream specified 
    *       in the Stream Name field is played beginning from the time specified in the 
    *       Start field.
    * If no recorded stream is found, the next item in the playlist is played.
    */
    double start;
    /**
    * An optional parameter that specifies the duration of playback in seconds.
    * The default value is -1. The -1 value means a live stream is played until it is no
    *       longer available or a recorded stream is played until it ends.
    * If u pass 0, it plays the single frame since the time specified in the Start field 
    *       from the beginning of a recorded stream. It is assumed that the value specified 
    *       in the Start field is equal to or greater than 0.
    * If you pass a positive number, it plays a live stream for the time period specified 
    *       in the Duration field. After that it becomes available or plays a recorded 
    *       stream for the time specified in the Duration field. (If a stream ends before the
    *       time specified in the Duration field, playback ends when the stream ends.)
    * If you pass a negative number other than -1 in the Duration field, it interprets the 
    *       value as if it were -1.
    */
    double duration;
    /**
    * An optional Boolean value or number that specifies whether to flush any
    * previous playlist.
    */
    bool reset;

public:
	rtmpplaypacket(){
		command_name = RTMP_AMF0_COMMAND_PLAY;
	    transaction_id = 0;
	    command_object = Amf0Any::null();

	    start = -2;
	    duration = -1;
	    reset = true;
	}
	~rtmpplaypacket(){}
	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream){}
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
private:
	int get_size(){
		int size = Amf0Size::str(command_name) + Amf0Size::number()
	        + Amf0Size::null() + Amf0Size::str(stream_name);
	    
	    if (start != -2 || duration != -1 || !reset) {
	        size += Amf0Size::number();
	    }
	    
	    if (duration != -1 || !reset) {
	        size += Amf0Size::number();
	    }
	    
	    if (!reset) {
	        size += Amf0Size::boolean();
	    }
	    
	    return size;
	}	
};


class rtmpcreatestreampacket : public rtmpcommonpacket
{
public:
	rtmpcreatestreampacket(){}
	~rtmpcreatestreampacket(){}
	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream)
	{

	}
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number() + Amf0Size::null();
	}
	
public:
	string command_name;
	double transaction_id;
};

class rtmpsampleacesspacket : public rtmpcommonpacket
{
public:
	rtmpsampleacesspacket(){
		 command_name = RTMP_AMF0_DATA_SAMPLE_ACCESS;
    	video_sample_access = false;
    	audio_sample_access = false;
	}
	
	~rtmpsampleacesspacket(){}
	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0DataMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetStream;
	}
private:
	int get_size(){
		return Amf0Size::str(command_name)
        	+ Amf0Size::boolean() + Amf0Size::boolean();
	}	
public:
	string command_name;
	bool video_sample_access;
	bool audio_sample_access;
};

class rtmponstatusdatapacket : public rtmpcommonpacket
{
public:
	rtmponstatusdatapacket(){
		command_name = RTMP_AMF0_COMMAND_ON_STATUS;
		data = new amf0object();
	}
	~rtmponstatusdatapacket(){}
	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0DataMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetStream;
	}
private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::object(data);
	}	
public:
	string command_name;
	amf0object* data;
};

class rtmpusercontrolmessage : public rtmpcommonpacket
{
public:
	rtmpusercontrolmessage(){
		event_type = 0;
    	event_data = 0;
    	extra_data = 0;
	}
	~rtmpusercontrolmessage(){}

	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream){}
	int get_message_type(){
		return RTMP_MSG_UserControlMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_PROTOCOLCONTROL;
	}
private:
	int get_size(){
		 int size = 2;
    
	    if (event_type == SrsPCUCFmsEvent0) {
	        size += 1;
	    } else {
	        size += 4;
	    }
	    
	    if (event_type == SrcPCUCSetBufferLength) {
	        size += 4;
	    }
	    
	    return size;
	}
public:
    /**
    * Event type is followed by Event data.
    * @see: SrcPCUCEventType
    */
    short event_type;
    /**
     * the event data generally in 4bytes.
     * @remark for event type is 0x001a, only 1bytes.
     * @see SrsPCUCFmsEvent0
     */
    int event_data;
    /**
    * 4bytes if event_type is SetBufferLength; otherwise 0.
    */
    int extra_data;
};

class rtmpconnectpacket : public rtmpcommonpacket
{
public:
	rtmpconnectpacket(){
		command_object = new amf0object();

	}
	~rtmpconnectpacket(){
		if(command_object)
		{
			delete command_object;
			command_object = NULL;
		}
	}

	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream)
	{

	}
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
private:
	int get_size(){
		return 4;
	}
	
public:
	string command_name;
	double transaction_id;
public:
	amf0object* command_object;
};

class rtmpconnectresponsepacket : public rtmpcommonpacket
{
public:
	rtmpconnectresponsepacket():command_name(RTMP_AMF0_COMMAND_RESULT),
								transaction_id(1){
		m_prop = (amf0object*)Amf0Any::object();
		m_info = (amf0object*)Amf0Any::object();
	}
	~rtmpconnectresponsepacket(){

	}

	int encode_packet(streamworker* stream);

	int decodepayload(rtmpcommonmessage* msg){

	}

	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}

	int get_prefer_chunkid(){
		return RTMP_CID_NetConnection;
	}
	
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number() + 
		    	Amf0Size::object(m_prop) + Amf0Size::object(m_info);
	}

public:
	string command_name;
	int transaction_id;

	amf0object *m_prop;
	amf0object *m_info;
	
};

class rtmpUnPublishPacket : public rtmpcommonpacket
{
public:
	rtmpUnPublishPacket() :command_name("FCUnpublish"), transaction_id(0)
	{
	}
	~rtmpUnPublishPacket(){};

	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream){};
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_ONSTATUS;
	}
private:
	int get_size(){
		return Amf0Size::str(RTMP_AMF0_COMMAND_UNPUBLISH) + Amf0Size::number()
			+ Amf0Size::null() + Amf0Size::str(stream_name);
	}
public:
	string command_name;
	string stream_name;
	double transaction_id;
};

class rtmpOnStatusCallPacket : public rtmpcommonpacket
{
public:
	rtmpOnStatusCallPacket(){
		data = new amf0object();
		transaction_id = 0;
	}
	~rtmpOnStatusCallPacket(){}

	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_ONSTATUS;
	}
private:
	int get_size(){
		return Amf0Size::str(RTMP_AMF0_COMMAND_ON_STATUS) + Amf0Size::number()
        + Amf0Size::null() + Amf0Size::object(data);
	}
public:
	amf0object* data;
	int transaction_id;

};

#if 0
class onstatusrespacket : public rtmpcommonpacket
{
public:
	onstatusrespacket():command_name("onStatus"),transaction_id(0){
		data = new amf0object();
	}
	~onstatusrespacket(){}
	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_AMF0CommandMessage;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_NetStream;
	}
private:
	int get_size(){
		return Amf0Size::str(command_name) + Amf0Size::number()
        + Amf0Size::null() + Amf0Size::object(data);
	}
public:
	string command_name;
	amf0object* data;
	int transaction_id;
};
#endif

class rtmpsetchunksizepacket : public rtmpcommonpacket
{
public:
	rtmpsetchunksizepacket():setchunksize(0)
	{

	}
	~rtmpsetchunksizepacket()
	{

	}

	int decodepayload(rtmpcommonmessage* msg);
	int encode_packet(streamworker* stream){}
	int get_message_type(){
		return RTMP_MSG_SetChunkSize;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_PROTOCOLCONTROL;
	}
private:
	int get_size(){
		return 4;
	}
public:
	int setchunksize;
	
};

class usercontrolpacket : public rtmpcommonpacket
{
public:
	usercontrolpacket(){
		event_type = 0;
		event_data = 0;
		extra_data = 0;
	}
	~usercontrolpacket(){}
	int decodepayload(rtmpcommonmessage* msg){}
	int encode_packet(streamworker* stream);
	int get_message_type(){
		return RTMP_MSG_UserControlMessage;
	}
	int get_prefer_chunkid(){
		return RTMP_CID_PROTOCOLCONTROL;
	}
private:
	int get_size(){
		int size = 2;
    
	    if (event_type == SrsPCUCFmsEvent0) {
	        size += 1;
	    } else {
	        size += 4;
	    }
	    
	    if (event_type == SrcPCUCSetBufferLength) {
	        size += 4;
	    }
	    
	    return size;
	}

public:
    /**
    * Event type is followed by Event data.
    * @see: SrcPCUCEventType
    */
    short event_type;
    /**
     * the event data generally in 4bytes.
     * @remark for event type is 0x001a, only 1bytes.
     * @see SrsPCUCFmsEvent0
     */
    int event_data;
    /**
    * 4bytes if event_type is SetBufferLength; otherwise 0.
    */
    int extra_data;
};


class rtmpcommonmessageheader
{
public:
	rtmpcommonmessageheader():timestamp_delta(0),
	                          payload_length(0),
	                          message_type(-1),
	                          stream_id(-1),
	                          timestamps(0),
	                          chunkid(-1)
	{

	}
	~rtmpcommonmessageheader(){

	}

    
public:
	int timestamp_delta; //if not type 0 , message header need delta time 
	int payload_length;
	int message_type;
	int stream_id;
	int timestamps;  //uf type is 0 , message header need timestamp
	int chunkid;
};

class rtmpcommonmessage
{
public:
	rtmpcommonmessage()
	{

	}
	~rtmpcommonmessage()
	{
		if(payload != NULL)
		{
			delete[] payload;
			payload = NULL;
		}
	}
public:
	int size;
	char* payload;
};


class chunkstream
{
public:
	chunkstream(int m_cid){
		msg = new rtmpcommonmessage();
		chunk_count = 0;
		cid = m_cid;
		header.chunkid = m_cid;
		msg = NULL;
	}
	~chunkstream(){
	}

	rtmpcommonmessageheader header;
	bool extended_timestamp;
	rtmpcommonmessage *msg;
	int cid;
	int chunk_count;
};

typedef enum
{
    simple = 0,
    complex
}HandType;


class rtmpprotocol
{
public:
	rtmpprotocol(int fd);
	~rtmpprotocol();

	int handle_handshakeC0C1();
	int handle_handshakeC2();
	int connectapp();
	int set_window_ack_size(int ack_size);
	int set_peer_bandwidth_size(int bandwidth, char type);
	int set_bwdown();
	int set_chunk_size(int size);
	int response_connect_app();
	int stream_service_cycle();
	int pull_and_send_stream();
private:
	void add_play_client(int clientfd);
private:
	int readC0C1();
	char* getC0C1(){
		if (m_c0c1)
			return m_c0c1;
		else
			return NULL;
	}
	int createS0S1S2(c1s1& _c1s1);
	void setHandShakeMethod(HandType type)
	{
		m_handtype = type;
	}
	int readC2();
	char* getC2()
	{
		if (m_c2)
		{
			return m_c2;
		}
		else
			return NULL;
	}
	int recv_message(rtmpcommonmessage** msg);
	int recv_interlaced_message(rtmpcommonmessage** msg, rtmpcommonmessageheader& header,int& chunkfmt);
	int readbasicheader(char& fmt, int& cid);
	int readmessageheader(chunkstream* chunk,char fmt);
	int readmessagepayload(chunkstream* chunk,char fmt);
	int distinguishusercontrolmessage(rtmpcommonmessage*);
	int distinguishpacket(rtmpcommonmessage*,int,int);
	int on_audiodata(rtmpcommonmessage* msg, rtmpcommonmessageheader header,int fmt);
	int on_videodata(rtmpcommonmessage* msg, rtmpcommonmessageheader header,int fmt);
	int on_amfmessagedata(rtmponmetapacket *packet,rtmpcommonmessage* msg, rtmpcommonmessageheader header,int fmt, int amftype);
	int send_and_free_packet(rtmpcommonpacket* packet, int stream_id);
	int send_and_free_message_to_client(sharedMessage* msg);
	int send_and_free_message_to_client(sharedMessage* msg, int clientfd);
	void send_gop_cache(gopcache& cache);
	int do_packet_send(rtmpcommonmessageheader* mh, char* payload, int size);
	int create_c0_chunk(char* chunk,sharedMessageheader* mh);
    int create_c0_chunk(char* chunk,rtmpcommonmessageheader* mh);
	int create_c1_chunk(char* chunk,rtmpcommonmessageheader* mh);
	int create_c2_chunk(char* chunk,rtmpcommonmessageheader* mh);
	int create_c3_chunk(char* chunk,sharedMessageheader* mh);
    int create_c3_chunk(char* chunk,rtmpcommonmessageheader* mh);
	
	void remove_play_client(int fd);
private:
	requestinfo req;
	rtmpstreamsource* m_source;
	char* m_c0c1;
	char* m_s0s1s2;
	char* m_c2;
	int m_fd;

	int default_chunksize;
	gopcache m_cache;	
	//chunkstream map for search every chunk with cid
	map<int,chunkstream*> m_chunkstreammap;
    HandType m_handtype;
	/*
	 * if the rtmpportocl is belong to publish streamsource , need us clientvec to record
	 * how many play client need it, and when video and audio data has come, it should send 
	 * them to all client in clientVec
	 */
	list<int> m_clientVec;  
	
	//chunk message 成员
//	int m_fmt;
//	int m_cid;
};


#endif
