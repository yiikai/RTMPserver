#ifndef STREAM_SOURCE_H
#define STREAM_SOURCE_H
#include <map>
#include <requestinfo.h>
#include <queue>
using namespace std;

class sharedMessageheader;
class sharedMessage;

class rtmpstreamsource
{
public:
	rtmpstreamsource(){}
	~rtmpstreamsource(){}
public:
	void push_msg(sharedMessage* msg);
	void pop_msg();
	sharedMessage* get_front();
private:
	queue<sharedMessage*> m_sharequeue;
	
};

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
	char* data;
};

class sharedMessage
{
public:
	sharedMessage(){}
	~sharedMessage(){}
	
public:
	sharedMessageheader header;
	char* payload;

	char* messagepalyload;
	int length;
	
};

class streamsource
{
public:
	~streamsource();
	rtmpstreamsource* findsource(requestinfo* req);
	rtmpstreamsource* createsource(requestinfo* req);
	static streamsource* getInstance();
private:
	streamsource();
private:
	static streamsource m_instance;
	static map<string , rtmpstreamsource*> m_pool;
};


#endif