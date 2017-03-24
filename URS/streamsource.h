#ifndef STREAM_SOURCE_H
#define STREAM_SOURCE_H
#include <map>
#include <requestinfo.h>
#include <queue>
#include <pthread.h>  
using namespace std;

class rtmpprotocol;


class sharedMessageheader
{
public:
	sharedMessageheader():timestamp_delta(0),
								payload_length(0),
								message_type(0),
								stream_id(0),
								timestamps(0),
								chunkid(0)
								{}
	~sharedMessageheader(){}
public:
	int timestamp_delta;  
	int payload_length;
	int message_type;
	int stream_id;
	int timestamps; 
	int chunkid;
};

class sharedMessage
{
public:
	sharedMessage(){}
	~sharedMessage(){}
	sharedMessage(const sharedMessage& message);
	sharedMessage& operator=(const sharedMessage& message);
public:
	sharedMessageheader header;
	char* payload;

	char* messagepalyload;
	int length;
	
};


class rtmpstreamsource
{
public:
	rtmpstreamsource(rtmpprotocol* rp):m_rp(NULL){
		m_rp = rp;
		pthread_mutex_init(&m_tsmux_mutex,NULL);
	}   //将相关连的protocol和source关联起来，方便与streamsource的client的管理
	~rtmpstreamsource(){}
public:
	void push_msg(sharedMessage* msg);
	void pop_msg();

	sharedMessage* tsmuxer_front();
	void push_tsmuxmsg(sharedMessage* msg);
	void pop_tsmuxmsg();
	void record_MessageData(const sharedMessage& message);
	void record_videoSequenceHead(const sharedMessage& message);
	sharedMessage& get_MessageData()
	{
		return m_messagedata;
	}
	sharedMessage& get_Video_Sequence_Head()
	{
		return m_videosequnecehead;
	}

	sharedMessage* get_front();
	rtmpprotocol* get_rtmpprotocol(){
		if(!m_rp)
			return NULL;
		return m_rp;
	}
	pthread_mutex_t m_tsmux_mutex;
private:
	queue<sharedMessage*> m_sharequeue;
	queue<sharedMessage*> m_tsmuxerqueue;  //made for tsmuxer, when create ts, we need send message data to ffmpeg aviocontext
	sharedMessage        m_messagedata;
	sharedMessage        m_videosequnecehead;
	rtmpprotocol*        m_rp;
};


class streamsource
{
public:
	~streamsource();
	rtmpstreamsource* findsource(requestinfo* req);
	rtmpstreamsource* createsource(requestinfo* req,rtmpprotocol* cp);
	static streamsource* getInstance();
private:
	streamsource();
private:
	static streamsource m_instance;
	static map<string , rtmpstreamsource*> m_pool;
};


#endif
