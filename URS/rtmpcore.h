#define RTMP_SIG_SRS_KEY "SRS"
#define RTMP_SIG_SRS_CODE "maoyikai"
#define RTMP_SIG_SRS_AUTHROS "maoyikai"
// contact info.
#define RTMP_SIG_SRS_WEB "http://ossrs.net"
#define RTMP_SIG_SRS_EMAIL "winlin@vip.126.com"
// debug info.
#define RTMP_SIG_SRS_ROLE "cluster"
#define RTMP_SIG_SRS_NAME "(Simple RTMP Server)"
#define RTMP_SIG_SRS_URL_SHORT "github.com/ossrs/srs"
#define RTMP_SIG_SRS_URL "https://github.com/ossrs/srs"
#define RTMP_SIG_SRS_LICENSE "The MIT License (MIT)"
#define RTMP_SIG_SRS_COPYRIGHT "Copyright (c) 2013-2015 SRS(ossrs)"
#define RTMP_SIG_SRS_PRIMARY RTMP_SIG_SRS_KEY"/"VERSION_STABLE_BRANCH
#define RTMP_SIG_SRS_CONTRIBUTORS_URL RTMP_SIG_SRS_URL"/blob/master/AUTHORS.txt"
#define RTMP_SIG_SRS_HANDSHAKE RTMP_SIG_SRS_KEY"("RTMP_SIG_SRS_VERSION")"
#define RTMP_SIG_SRS_RELEASE RTMP_SIG_SRS_URL"/tree/"VERSION_STABLE_BRANCH".0release"
#define RTMP_SIG_SRS_ISSUES(id) RTMP_SIG_SRS_URL"/issues/"#id
#define RTMP_SIG_SRS_VERSION SRS_XSTR(VERSION_MAJOR)"."SRS_XSTR(VERSION_MINOR)"."SRS_XSTR(VERSION_REVISION)
#define RTMP_SIG_SRS_SERVER RTMP_SIG_SRS_KEY"/"RTMP_SIG_SRS_VERSION"("RTMP_SIG_SRS_CODE")"
#define SRS_XSTR(v) SRS_INTERNAL_STR(v)
#define SRS_INTERNAL_STR(v) #v