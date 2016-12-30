#ifndef AMF_0_H
#define AMF_0_H

#include "streamworker.h"
#include <vector>
#include <map>
#include <string>
#include <utility>
using namespace std;

class Amf0Any;
class Amf0Size;
class amf0object;

extern int srs_amf0_read_string(streamworker* stream, string& value);
extern int srs_amf0_read_utf8(streamworker* stream, string& value);
extern int srs_amf0_read_number(streamworker* stream, double& value);
extern int srs_amf0_read_any(streamworker* stream, Amf0Any** ppvalue);
extern int srs_amf0_read_null(streamworker* stream);
extern int srs_amf0_read_undefined(streamworker* stream);
extern int srs_amf0_read_boolean(streamworker* stream, bool& value);

extern int srs_amf0_write_string(streamworker* stream, string value);
extern int srs_amf0_write_utf8(streamworker* stream, string value);
extern int srs_amf0_write_any(streamworker* stream, Amf0Any* value);
extern int srs_amf0_write_number(streamworker* stream, double value);
extern int srs_amf0_write_null(streamworker* stream);
extern int srs_amf0_write_undefined(streamworker* stream);
extern int srs_amf0_write_boolean(streamworker* stream, bool value);

class Amf0Size
{
public:
	static int utf8(string value);
    static int str(string value);
    static int number();
    static int object(amf0object* obj);
    static int object_eof();
    static int any(Amf0Any* o);
	static int boolean();
    static int null();
    static int undefined();
};

class Amf0Any
{
public:
	Amf0Any();
	virtual ~Amf0Any();
	static int discovery(streamworker* stream, Amf0Any** ppvalue);
	static Amf0Any* str(const char* value = NULL);
	static Amf0Any* number(double value=0);
	static Amf0Any* boolean(bool value=false);
	static Amf0Any* object();
	static Amf0Any* null();
	virtual int read(streamworker* stream) = 0;
	virtual int write(streamworker* stream) = 0;
	virtual int totoalsize() = 0;

    virtual bool is_object();
    virtual amf0object* to_object();
public:
    char marker;
};

class Amf0Null : public Amf0Any
{
   public:
        Amf0Null();
       ~Amf0Null();
   public:
        int totoalsize();
        int read(streamworker* stream);
        int write(streamworker* stream);

 };

class amf0object : public Amf0Any
{
public:
	amf0object();
	~amf0object();

	int read(streamworker* stream);
	int write(streamworker* stream);
	void set_prop_keyvalue(string key,Amf0Any* value);
	int get_prop(string key,Amf0Any** value);

	int totoalsize();
private:
	vector< pair<string,Amf0Any*> > m_prop;
	map<string, string> m_getprop;

};


class Amf0boolean : public Amf0Any
{
public:
	Amf0boolean(bool value);
	~Amf0boolean();

	int read(streamworker* stream);
	int write(streamworker* stream);
	int totoalsize(){
		return Amf0Size::boolean();
	}
public:
	bool value;	
};

class Amf0String : public Amf0Any
{
 public:
 	Amf0String(const char* _value);
 	~Amf0String();

 	int read(streamworker* stream);
 	int write(streamworker* stream);
 	int totoalsize(){
 		return Amf0Size::str(value);
 	}
 public:
    std::string value;
};

class Amf0Number : public Amf0Any{
public:
	Amf0Number(double value);
	~Amf0Number();

	int read(streamworker* stream);
	int write(streamworker* stream);
	int totoalsize(){
		return Amf0Size::number();
	}
public:
	double value;
};


class Amf0EcmaArray : public Amf0Any{
public:
	Amf0EcmaArray(){}
	~Amf0EcmaArray(){}
	void set_prop(string key,Amf0Any* value)
	{
		m_prop.push_back(make_pair<string,Amf0Any*>(key,value));
	}
	int write(streamworker* stream);
	int read(streamworker* stream){}
	int totoalsize();
public:
	vector< pair<string,Amf0Any*> > m_prop;

};



#endif
