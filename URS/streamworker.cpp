#include "streamworker.h"
#include "string.h"
#include <stdio.h>
streamworker::streamworker():startpos(NULL),
							 currentpos(NULL),
							 size(0)
{

}

streamworker::~streamworker()
{

}

void streamworker::init(char* buf, int bufsize)
{
	startpos = buf;
	currentpos = buf;
	size = bufsize;
	endpos = startpos + size;
}

void streamworker::write1bytes(char value)
{
	*currentpos = value;
	currentpos++;	
}

void streamworker::write2bytes(short value)
{
    char* pp = (char*)&value;
    *currentpos++ = pp[1];
    *currentpos++ = pp[0];
}


void streamworker::write3bytes(int value)
{
	char* pp = (char*)&value;
	*currentpos++ = pp[2];
	*currentpos++ = pp[1];
	*currentpos++ = pp[0];
}

void streamworker::write4bytes(int value)
{
	char* pp = (char*)&value;
    *currentpos++ = pp[3];
    *currentpos++ = pp[2];
    *currentpos++ = pp[1];
    *currentpos++ = pp[0];
}


void streamworker::write8bytes(long long value)
{
    char* pp = (char*)&value;
    *currentpos++ = pp[7];
    *currentpos++ = pp[6];
    *currentpos++ = pp[5];
    *currentpos++ = pp[4];
    *currentpos++ = pp[3];
    *currentpos++ = pp[2];
    *currentpos++ = pp[1];
    *currentpos++ = pp[0];
}

void streamworker::write_stream(const char* stream,int streamsize)
{
	for(int i = 0; i<streamsize;i++)
		*currentpos++ = stream[i]; 
}


char streamworker::read_1byte()
{
	char data = *currentpos++;
	return data;
}

short streamworker::read_2bytes()
{
	short value;
    char* pp = (char*)&value;
    pp[1] = *currentpos++;
    pp[0] = *currentpos++;
    
    return value;
}

int streamworker::read_3bytes()
{
	int value = 0x00;
    char* pp = (char*)&value;
    pp[2] = *currentpos++;
    pp[1] = *currentpos++;
    pp[0] = *currentpos++;
    
    return value;
}

string streamworker::read_string(int len)
{
	std::string value;
    value.append(currentpos, len);
    
    currentpos += len;
    
    return value;
}

bool streamworker::require(int required_size)
{
	return required_size <= endpos - currentpos;
}

long long streamworker::read_8bytes()
{    
    long long value;
    char* pp = (char*)&value;
    pp[7] = *currentpos++;
    pp[6] = *currentpos++;
    pp[5] = *currentpos++;
    pp[4] = *currentpos++;
    pp[3] = *currentpos++;
    pp[2] = *currentpos++;
    pp[1] = *currentpos++;
    pp[0] = *currentpos++;
    
    return value;
}

void streamworker::read_bytes(char* data,int size)
{
    memcpy(data,currentpos,size);
    currentpos += size;
}


int streamworker::read_4bytes()
{
	int value;
    char* pp = (char*)&value;
    pp[3] = *currentpos++;
    pp[2] = *currentpos++;
    pp[1] = *currentpos++;
    pp[0] = *currentpos++;
    
    return value;
}

bool streamworker::isempty()
{
	if (currentpos == endpos)
	{
		return true;
	}
	return false;
}

void streamworker::streamback(int size)
{
	currentpos += size;
}