#include "amf0.h"
#include <string.h>
#include <stdio.h>

#define RTMP_AMF0_Number                     0x00
#define RTMP_AMF0_Boolean                     0x01
#define RTMP_AMF0_String                     0x02
#define RTMP_AMF0_Object                     0x03
#define RTMP_AMF0_MovieClip                 0x04 // reserved, not supported
#define RTMP_AMF0_Null                         0x05
#define RTMP_AMF0_Undefined                 0x06
#define RTMP_AMF0_Reference                 0x07
#define RTMP_AMF0_EcmaArray                 0x08
#define RTMP_AMF0_ObjectEnd                 0x09
#define RTMP_AMF0_StrictArray                 0x0A
#define RTMP_AMF0_Date                         0x0B
#define RTMP_AMF0_LongString                 0x0C
#define RTMP_AMF0_UnSupported                 0x0D
#define RTMP_AMF0_RecordSet                 0x0E // reserved, not supported
#define RTMP_AMF0_XmlDocument                 0x0F
#define RTMP_AMF0_TypedObject                 0x10
// AVM+ object is the AMF3 object.
#define RTMP_AMF0_AVMplusObject             0x11
// origin array whos data takes the same form as LengthValueBytes
#define RTMP_AMF0_OriginStrictArray         0x20

// User defined
#define RTMP_AMF0_Invalid                     0x3F



int srs_amf0_read_string(streamworker* stream, string& value)
{
    int ret = 0;
    
    char marker = stream->read_1byte();
    if (marker != RTMP_AMF0_String) {
        ret = -1;
        printf("amf0 check string marker failed. "
		"marker=%#x, required=%#x, ret=%d\n", marker, RTMP_AMF0_String, ret);
        return ret;
    }
    //printf("amf0 read string marker success");
    
    return srs_amf0_read_utf8(stream, value);
}

int srs_amf0_read_utf8(streamworker* stream, string& value)
{
        int ret = 0;
        
        // len
        
        short len = stream->read_2bytes();
        //printf("amf0 read string length success. len=%d\n", len);
        
        // empty string
        if (len <= 0) {
            ret = -1;
            //printf("amf0 read empty string. ret=%d\n", ret);
            return ret;
        }
        
        // data
        
        std::string str = stream->read_string(len);
        
        value = str;
        //printf("amf0 read string data success. str=%s\n", str.c_str());
        
        return ret;
}

int srs_amf0_read_number(streamworker* stream, double& value)
{
    int ret = 0;
    
    char marker = stream->read_1byte();
    if (marker != RTMP_AMF0_Number) {
        ret = -1;
        printf("amf0 check number marker failed. "
            "marker=%#x, required=%#x, ret=%d\n", marker, RTMP_AMF0_Number, ret);
        return ret;
    }
    //printf("amf0 read number marker success\n");

    // value
    long long temp = stream->read_8bytes();
    memcpy(&value, &temp, 8);
    
    //printf("amf0 read number value success. value=%.2f\n", value);
    
    return ret;
}


int srs_amf0_read_boolean(streamworker* stream, bool& value)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 read bool marker failed. ret=%d\n", ret);
        return ret;
    }
    
    char marker = stream->read_1byte();
    if (marker != RTMP_AMF0_Boolean) {
        ret = -1;
        printf("amf0 check bool marker failed. "
            "marker=%#x, required=%#x, ret=%d\n", marker, RTMP_AMF0_Boolean, ret);
        return ret;
    }
    //printf("amf0 read bool marker success\n");

    // value
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 read bool value failed. ret=%d\n", ret);
        return ret;
    }

    value = (stream->read_1byte() != 0);
    
    //printf("amf0 read bool value success. value=%d\n", value);
    
    return ret;
}

int srs_amf0_read_undefined(streamworker* stream)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 read undefined marker failed. ret=%d\n", ret);
        return ret;
    }
    
    char marker = stream->read_1byte();
    if (marker != RTMP_AMF0_Undefined) {
        ret = -1;
        printf("amf0 check undefined marker failed. "
            "marker=%#x, required=%#x, ret=%d\n", marker, RTMP_AMF0_Undefined, ret);
        return ret;
    }
    //printf("amf0 read undefined success\n");
    
    return ret;
}

int srs_amf0_read_null(streamworker* stream)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 read null marker failed. ret=%d\n", ret);
        return ret;
    }
    
    char marker = stream->read_1byte();
    if (marker != RTMP_AMF0_Null) {
        ret = -1;
        printf("amf0 check null marker failed. "
            "marker=%#x, required=%#x, ret=%d\n", marker, RTMP_AMF0_Null, ret);
        return ret;
    }
    //printf("amf0 read null success\n");
    
    return ret;
}

bool srs_amf0_is_object_eof(streamworker* stream) 
{
    // detect the object-eof specially
    if (stream->require(3)) {
        int flag = stream->read_3bytes();
        stream->streamback(-3);
        return 0x09 == flag;
    }
        
    return false;
}


int srs_amf0_read_any(streamworker* stream, Amf0Any** ppvalue)
{
    int ret = 0;

    if ((ret = Amf0Any::discovery(stream, ppvalue)) != 0) {
        //printf("amf0 discovery any elem failed. ret=%d\n", ret);
        return ret;
    }

    if ((ret = (*ppvalue)->read(stream)) != 0) {
        //printf("amf0 parse elem failed. ret=%d\n", ret);
        return ret;
    }
    
    return ret;
}

int srs_amf0_write_boolean(streamworker* stream, bool value)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write bool marker failed. ret=%d\n", ret);
        return ret;
    }
    stream->write1bytes(RTMP_AMF0_Boolean);
    //printf("amf0 write bool marker success");

    // value
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write bool value failed. ret=%d\n", ret);
        return ret;
    }

    if (value) {
        stream->write1bytes(0x01);
    } else {
        stream->write1bytes(0x00);
    }
    
    //printf("amf0 write bool value success. value=%d\n", value);
    
    return ret;
}

int srs_amf0_write_string(streamworker* stream, string value)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write string marker failed. ret=%d\n", ret);
        return ret;
    }
    
    stream->write1bytes(RTMP_AMF0_String);
    //printf("amf0 write string marker success");
    
    return srs_amf0_write_utf8(stream, value);
}


 int srs_amf0_write_utf8(streamworker* stream, string value)
    {
        int ret = 0;
        
        // len
        if (!stream->require(2)) {
            ret = -1;
            //printf("amf0 write string length failed. ret=%d\n", ret);
            return ret;
        }
        stream->write2bytes(value.length());
        //printf("amf0 write string length success. len=%d\n", (int)value.length());
        
        // empty string
        if (value.length() <= 0) {
            //printf("amf0 write empty string. ret=%d\n", ret);
            return ret;
        }
        
        // data
        if (!stream->require(value.length())) {
            ret = -1;
            //printf("amf0 write string data failed. ret=%d\n", ret);
            return ret;
        }
        stream->write_stream(value.c_str(),value.length());
        //printf("amf0 write string data success. str=%s\n", value.c_str());
        
        return ret;
    }


int srs_amf0_write_number(streamworker* stream, double value)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write number marker failed. ret=%d\n", ret);
        return ret;
    }
    
    stream->write1bytes(RTMP_AMF0_Number);
    //printf("amf0 write number marker success");

    // value
    if (!stream->require(8)) {
        ret = -1;
        //printf("amf0 write number value failed. ret=%d\n", ret);
        return ret;
    }

    long long temp = 0x00;
    memcpy(&temp, &value, 8);
    stream->write8bytes(temp);
    
    //printf("amf0 write number value success. value=%.2f\n", value);
    
    return ret;
}


int srs_amf0_write_null(streamworker* stream)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write null marker failed. ret=%d\n", ret);
        return ret;
    }
    
    stream->write1bytes(RTMP_AMF0_Null);
    //printf("amf0 write null marker success\n");
    
    return ret;
}

int srs_amf0_write_undefined(streamworker* stream)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write undefined marker failed. ret=%d\n", ret);
        return ret;
    }
    
    stream->write1bytes(RTMP_AMF0_Undefined);
    //printf("amf0 write undefined marker success\n");
    
    return ret;
}

int srs_amf0_write_any(streamworker* stream, Amf0Any* value)
{
    return value->write(stream);
}

amf0object::amf0object()
{
    marker = RTMP_AMF0_Object;
}

amf0object::~amf0object()
{
	vector< pair<string,Amf0Any*> >::iterator itr = m_prop.begin();
	for(;itr != m_prop.end();itr++)
	{
		if(itr->second)
			delete itr->second;
	}
	m_prop.clear();
	m_getprop.clear();
}

void amf0object::set_prop_keyvalue(string key,Amf0Any* value)
{
    m_prop.push_back(make_pair<string,Amf0Any*>(key,value));
}

int amf0object::totoalsize()
{
    int size = 1;
    vector< pair<string,Amf0Any*> >::iterator itr = m_prop.begin();
    for (; itr != m_prop.end() ;itr++){
        std::string name = itr->first;
        Amf0Any* value = itr->second;
        
        size += Amf0Size::utf8(name);
        size += Amf0Size::any(value);
    }
    
    size += Amf0Size::object_eof();
    
    return size;
}

int amf0object::read(streamworker* stream)
{
    int ret = 0;
    char marker = stream->read_1byte();
    if (marker != RTMP_AMF0_Object) {
        ret = -1;
        printf("amf0 check object marker failed. "
            "marker=%#x, required=%#x, ret=%d\n", marker, RTMP_AMF0_Object, ret);
        return ret;
    }
    //循环读取所有command-object
    while(!stream->isempty())
    {
        if (srs_amf0_is_object_eof(stream)) {
            //printf("amf0 read object EOF.\n");
            break;
        }
        std::string property_name;
        if ((ret = srs_amf0_read_utf8(stream, property_name)) != 0) {
            //printf("amf0 object read property name failed. ret=%d\n", ret);
            return ret;
        }

        Amf0Any* property_value = NULL;
        if ((ret = srs_amf0_read_any(stream, &property_value)) != 0) {
            printf("amf0 object read property_value failed. "
                "name=%s, ret=%d\n", property_name.c_str(), ret);
            return ret;
        }
        //获取object属性值存放起来，通用的方法还没有找到，继续改
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if 0
        if(property_name == "tcUrl")
        {
            m_prop.push_back(make_pair(property_name,property_value));
        }
        else if(property_name == "objectEncoding")
        {
            m_prop.push_back(make_pair(property_name,property_value));
        }
#endif
        m_prop.push_back(make_pair(property_name,property_value));



    }
    return ret;

}

int amf0object::get_prop(string key,Amf0Any** value)
{
    int ret = 0;
    vector< pair<string,Amf0Any*> >::iterator itr = m_prop.begin();
    for(;itr != m_prop.end(); itr++)
    {
        if(itr->first != key)
        {
            continue;
        }
        else
        {
            *value = itr->second;
            break;
        }
    }
    
    // map<string, string>::iterator itr;
    // itr = m_getprop.find(key);
    // if(itr == m_getprop.end())
    // {
    //     return -1;
    // }
    // value = itr->second;
    return 0;
}

int amf0object::write(streamworker* stream)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write object marker failed. ret=%d\n", ret);
        return ret;
    }
    
    stream->write1bytes(RTMP_AMF0_Object);
    //printf("amf0 write object marker success\n");

    vector< pair<string,Amf0Any*> >::iterator itr = m_prop.begin();
    for (; itr != m_prop.end(); itr++) {
        std::string name = itr->first;
        Amf0Any* any = itr->second;
        
        if ((ret = srs_amf0_write_utf8(stream, name)) != 0) {
            //printf("write object property name failed. ret=%d\n", ret);
            return ret;
        }
        
        if ((ret = srs_amf0_write_any(stream, any)) != 0) {
            //printf("write object property value failed. ret=%d\n", ret);
            return ret;
        }
        
        //printf("write amf0 property success. name=%s\n", name.c_str());
    }
    //Amf0 object need add 0x00 0x00 0x09 to be end flag
    stream->write2bytes(0x00);
    stream->write1bytes(0x09);
    return ret;
}

Amf0Any::Amf0Any()
{

}

Amf0Any::~Amf0Any()
{

}

int Amf0Any::discovery(streamworker* stream, Amf0Any** ppvalue)
{
    int ret = 0;
    
    // detect the object-eof specially
    if (srs_amf0_is_object_eof(stream)) {
        //printf("amf0 is ending \n");
        return -1;
    }

    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 read any marker failed. ret=%d\n", ret);
        return ret;
    }

    char marker = stream->read_1byte();
    //printf("amf0 any marker success\n");
    stream->streamback(-1);

    switch(marker)
    {
        case RTMP_AMF0_String: {
            *ppvalue = Amf0Any::str();
            return ret;
        }break;
        case RTMP_AMF0_Boolean: {
             *ppvalue = Amf0Any::boolean();
             return ret;
        }break;
        case RTMP_AMF0_Number: {
            *ppvalue = Amf0Any::number();
             return ret;
        }break;
        // case RTMP_AMF0_Null: {
        //     *ppvalue = Amf0Any::null();
        //     return ret;
        // }
        // case RTMP_AMF0_Undefined: {
        //     *ppvalue = Amf0Any::undefined();
        //     return ret;
        // }
        case RTMP_AMF0_Object: {
             *ppvalue = Amf0Any::object();
             return ret;
        }break;
        // case RTMP_AMF0_EcmaArray: {
        //     *ppvalue = Amf0Any::ecma_array();
        //     return ret;
        // }
        // case RTMP_AMF0_StrictArray: {
        //     *ppvalue = Amf0Any::strict_array();
        //     return ret;
        // }
        // case RTMP_AMF0_Date: {
        //     *ppvalue = Amf0Any::date();
        //     return ret;
        // }
        case RTMP_AMF0_Invalid:
        default: {
            ret = -1;
            //printf("invalid amf0 message type. marker=%#x, ret=%d\n", marker, ret);
            return ret;
        }
    }
    return ret;
}


Amf0Any* Amf0Any::str(const char* value)
{
    return new Amf0String(value);
}

Amf0Any* Amf0Any::number(double value)
{
    return  new Amf0Number(value);
}

Amf0Any* Amf0Any::object()
{
    return new amf0object();
}

 Amf0Any* Amf0Any::null()
 {
 	return new Amf0Null();
 }	

Amf0Any* Amf0Any::boolean(bool value)
{
	return new Amf0boolean(value);
}

bool Amf0Any::is_object()
{
    return marker == RTMP_AMF0_Object;
}

amf0object* Amf0Any::to_object()
{
    amf0object* p = dynamic_cast<amf0object*>(this);
    return p;
}

Amf0String::Amf0String(const char* _value):Amf0Any()
{
    marker = RTMP_AMF0_String;
    if (_value) {
        value = _value;
    }
}

Amf0String::~Amf0String()
{

}

int Amf0String::read(streamworker* stream)
{
    return srs_amf0_read_string(stream, value);
}

int Amf0String::write(streamworker* stream)
{
    return srs_amf0_write_string(stream,value);
}

Amf0Number::Amf0Number(double value)
{
    marker = RTMP_AMF0_Number;
    this->value = value;
}    

Amf0Number::~Amf0Number()
{

}

int Amf0Number::read(streamworker* stream)
{
    return srs_amf0_read_number(stream,value);
}

int Amf0Number::write(streamworker* stream)
{
    return srs_amf0_write_number(stream, value);
}


Amf0boolean::Amf0boolean(bool value)
{
	marker = RTMP_AMF0_Boolean;
    value = value;
}

Amf0boolean::~Amf0boolean()
{

}

int Amf0boolean::read(streamworker* stream)
{
	 return srs_amf0_read_boolean(stream,value);
}

int Amf0boolean::write(streamworker* stream)
{
	return srs_amf0_write_boolean(stream,value);
}


Amf0Null::Amf0Null()
{
	marker = RTMP_AMF0_Null;
}

Amf0Null::~Amf0Null()
{
	
}

int Amf0Null::totoalsize()
{
	return Amf0Size::null();
}

int Amf0Null::read(streamworker* stream)
{
	 return srs_amf0_read_null(stream);
}

int Amf0Null::write(streamworker* stream)
{
	return srs_amf0_write_null(stream);
}


int Amf0Size::utf8(string value)
{
     return 2 + value.length();
}

int Amf0Size::str(string value)
{
    return 1 + utf8(value);
}

int Amf0Size::object(amf0object* obj)
{
    if (!obj) {
        return 0;
    }
    return obj->totoalsize();
}   

int Amf0Size::boolean()
{
	return 2;
}
	

int Amf0Size::number()
{
    return 1 + 8;     //amf0header 0x01 and 1 byte
}

int Amf0Size::object_eof()
{   
     return 2 + 1;     //0x00 0x00 0x09
}   

int Amf0Size::any(Amf0Any* o)
{
    if (!o) {
        return 0;
    }
    
    return o->totoalsize();
}

int Amf0Size::null()
{
    return 1;
}

int Amf0Size::undefined()
{
    return 1;
}

int Amf0EcmaArray::write(streamworker* stream)
{
    int ret = 0;
    
    // marker
    if (!stream->require(1)) {
        ret = -1;
        //printf("amf0 write ecma_array marker failed. ret=%d\n", ret);
        return ret;
    }
    
    stream->write1bytes(RTMP_AMF0_EcmaArray);
    //printf("amf0 write ecma_array marker success\n");

    // count
    if (!stream->require(4)) {
        ret = -1;
        //printf("amf0 write ecma_array count failed. ret=%d\n", ret);
        return ret;
    }
    
    stream->write4bytes(m_prop.size());
    //printf("amf0 write ecma_array count success. count=%d\n", m_prop.size());
    
    // value
    for (int i = 0; i < m_prop.size(); i++) {
        std::string name = m_prop[i].first;
        Amf0Any* any = m_prop[i].second;
        
        if ((ret = srs_amf0_write_utf8(stream, name)) != 0) {
            //printf("write ecma_array property name failed. ret=%d\n", ret);
            return ret;
        }
        
        if ((ret = srs_amf0_write_any(stream, any)) != 0) {
            //printf("write ecma_array property value failed. ret=%d\n", ret);
            return ret;
        }
        
        //printf("write amf0 property success. name=%s\n", name.c_str());
    }
    
    stream->write2bytes(0x00);
    stream->write1bytes(0x09);
    //printf("write ecma_array object success.\n");
    
    return ret;
}   

int Amf0EcmaArray::totoalsize(){

    int size = 1 + 4;
    
    for (int i = 0; i < m_prop.size(); i++){
        std::string name = m_prop[i].first;
        Amf0Any* value = m_prop[i].second;
        
        size += Amf0Size::utf8(name);
        size += Amf0Size::any(value);
    }
    
    size += 3;
    
    return size;
}