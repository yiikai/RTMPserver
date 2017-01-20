#include "streamsource.h"
#include "rtmpprotocol.h"
#include <string.h>
streamsource streamsource::m_instance;
map<string , rtmpstreamsource*> streamsource::m_pool;


sharedMessageheader header;
	char* payload;

	char* messagepalyload;
	int length;


sharedMessage::sharedMessage(const sharedMessage& message)
{
	header = message.header;
	length = message.length;
	messagepalyload = new char[length];
	memcpy(messagepalyload,message.messagepalyload,length);
}
	
sharedMessage& sharedMessage::operator=(const sharedMessage& message)
{
	header = message.header;
	length = message.length;
	messagepalyload = new char[length];
	memcpy(messagepalyload,message.messagepalyload,length);
	return *this;
}




void rtmpstreamsource::push_msg(sharedMessage* msg)
{
	m_sharequeue.push(msg);
}

void rtmpstreamsource::pop_msg()
{
	m_sharequeue.pop();
}

void rtmpstreamsource::record_MessageData(const sharedMessage& message)
{
	m_messagedata = message;
}


void rtmpstreamsource::record_videoSequenceHead(const sharedMessage& message)
{
	m_videosequnecehead = message;
}


sharedMessage* rtmpstreamsource::get_front()
{
    if(m_sharequeue.empty())
    {
        return NULL;
    }
	return m_sharequeue.front();
}


streamsource::streamsource(){

}

streamsource::~streamsource(){

}

streamsource* streamsource::getInstance(){
	return &m_instance;
}

rtmpstreamsource* streamsource::findsource(requestinfo* req)
{
	map<string , rtmpstreamsource*>::iterator itr;
	itr = m_pool.find(req->tcUrl);
	if(itr == m_pool.end())
	{
		return NULL;
	}
	return itr->second;
}

rtmpstreamsource* streamsource::createsource(requestinfo* req,rtmpprotocol* rp)
{
	rtmpstreamsource* source = new rtmpstreamsource(rp);
	m_pool.insert(make_pair<string,rtmpstreamsource*>(req->tcUrl,source));
	return source;
}	
