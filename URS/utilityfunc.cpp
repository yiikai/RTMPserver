#include <stdlib.h>
#include <stdio.h>
#include "utilityfunc.h"
void random_generate(char* bytes, int size)
{
	static bool _random_initialized = false;
    if (!_random_initialized) {
        srand(0);
        _random_initialized = true;
    }
    
    for (int i = 0; i < size; i++) {
        // the common value in [0x0f, 0xf0]
        bytes[i] = 0x0f + (rand() % (256 - 0x0f - 0x0f));
    }
}

bool bytes_equals(void* pa, void* pb, int size)
{
    unsigned char* a = (unsigned char*)pa;
    unsigned char* b = (unsigned char*)pb;
    
    if (!a && !b) {
        return true;
    }
    
    if (!a || !b) {
        return false;
    }
    
    for(int i = 0; i < size; i++){
        if(a[i] != b[i]){
            return false;
        }
    }

    return true;
}

void srs_discovery_tc_url(
    string tcUrl, 
    string& schema, string& host, 
    string& app, string& port, string& param
) 
{
    size_t pos = std::string::npos;
    std::string url = tcUrl;
    
    if ((pos = url.find("://")) != std::string::npos) {
        schema = url.substr(0, pos);
        url = url.substr(schema.length() + 3);
        printf("discovery schema=%s\n", schema.c_str());
    }
    
    if ((pos = url.find("/")) != std::string::npos) {
        host = url.substr(0, pos);
        url = url.substr(host.length() + 1);
        printf("discovery host=%s\n", host.c_str());
    }

    port = 1935;
    if ((pos = host.find(":")) != std::string::npos) {
        port = host.substr(pos + 1);
        host = host.substr(0, pos);
        printf("discovery host=%s, port=%s\n", host.c_str(), port.c_str());
    }
    
    app = url;
}