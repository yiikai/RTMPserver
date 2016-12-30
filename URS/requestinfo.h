#ifndef REQUEST_INFO_H
#define REQUEST_INFO_H
#include <string>
#include <amf0.h>
class requestinfo
{
public:
	requestinfo(){}
	~requestinfo(){}
public:
	// discovery from tcUrl and play/publish.
    std::string schema;
    // the host in tcUrl.
    std::string host;
    // the port in tcUrl.
    std::string port;
    // the app in tcUrl, without param.
    std::string app;
    // the param in tcUrl(app).
    std::string param;
    // the stream in play/publish
    std::string stream;
    // for play live stream,
    // used to specified the stop when exceed the duration.
    // @see https://github.com/ossrs/srs/issues/45
    // in ms.
    double duration;
    // the token in the connect request,
    // used for edge traverse to origin authentication,
    // @see https://github.com/ossrs/srs/issues/104
    amf0object* args;

	std::string tcUrl;
    std::string pageUrl;
    std::string swfUrl;
    double objectEncoding;
};

#endif