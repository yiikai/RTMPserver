#include "streamsource.h"

streamsource streamsource::m_instance;
map<string , rtmpstreamsource*> streamsource::m_pool;


void rtmpstreamsource::push_msg(sharedMessage* msg)
{
	m_sharequeue.push(msg);
}

void rtmpstreamsource::pop_msg()
{
	m_sharequeue.pop();
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

rtmpstreamsource* streamsource::createsource(requestinfo* req)
{
	rtmpstreamsource* source = new rtmpstreamsource();
	m_pool.insert(make_pair<string,rtmpstreamsource*>(req->tcUrl,source));
	return source;
}	
