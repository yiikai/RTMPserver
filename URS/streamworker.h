#ifndef STREAM_WORKER_H
#define STREAM_WORKER_H
#include <string>
using namespace std;
class streamworker
{
public:
	streamworker();
	~streamworker();
	void init(char* buf, int bufsize);
	void write1bytes(char value);
	void write2bytes(short value);
	void write3bytes(int value);
	void write4bytes(int value);
	void write8bytes(long long vlaue);
	void write_stream(const char* stream,int streamsize);
	char* data(){ return startpos; }
	int pos(){ return currentpos - startpos; }
	int getsize(){ return size; }
	char read_1byte();
	short read_2bytes();
	int read_3bytes();
	int read_4bytes();
	long long read_8bytes();
    void read_bytes(char* data,int size);
	string read_string(int len);

	bool isempty();
	bool require(int required_size);
	void streamback(int size);
	char* getcurrentpos(){
		return currentpos;
	}
	int getcurrentlen(){
		return endpos - startpos;
	}
private:
	char* startpos;
	char* endpos;
	char* currentpos;
	int size;
};

#endif