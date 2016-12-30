#ifndef UTILITY_FUNC_H
#define UTILITY_FUNC_H
#include <string>
using namespace std;
extern void random_generate(char* bytes, int size);
extern void srs_discovery_tc_url(string tcUrl, 
    string& schema, string& host, 
    string& app, string& port, string& param);
extern bool bytes_equals(void* pa, void* pb, int size);


#endif