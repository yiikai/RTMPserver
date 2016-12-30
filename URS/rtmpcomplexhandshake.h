#ifndef RTMP_COMPLEX_HANDSHAKE_H
#define RTMP_COMPLEX_HANDSHAKE_H

#include <openssl/evp.h>
#include <openssl/hmac.h>
// for openssl_generate_key
#include <openssl/dh.h>
#include <openssl/hmac.h>

class streamworker;
class c1s1;

typedef enum{
    HAND_SHAKE_SCHEMA0 = 0,
    HAND_SHAKE_SCHEMA1,
    HAND_SHAKE_SIMPLE
}Hschem;

 #define OPENSSL_HASH_SIZE 512

#if 0

class rtmpcomplexhandshake
{
public:
    rtmpcomplexhandshake();
    ~rtmpcomplexhandshake();

};
#endif

class key_block
    {
    public:
        // (offset)bytes
        char* random0;
        int random0_size;
        
        // 128bytes
        char key[128];
        
        // (764-offset-128-4)bytes
        char* random1;
        int random1_size;
        
        // 4bytes
        int offset;
    public:
        key_block();
        virtual ~key_block();
    public:
        // parse key block from c1s1.
        // if created, user must free it by srs_key_block_free
        // @stream contains c1s1_key_bytes the key start bytes
        int parse(streamworker* stream);
    private:
        // calc the offset of key,
        // the key->offset cannot be used as the offset of key.
        int calc_valid_offset();
    };



class digest_block
    {
    public:
        // 4bytes
        int offset;
        
        // (offset)bytes
        char* random0;
        int random0_size;
        
        // 32bytes
        char digest[32];
        
        // (764-4-offset-32)bytes
        char* random1;
        int random1_size;
    public:
        digest_block();
        virtual ~digest_block();
    public:
        // parse digest block from c1s1.
        // if created, user must free it by srs_digest_block_free
        // @stream contains c1s1_digest_bytes the digest start bytes
        int parse(streamworker* stream);
    private:
        // calc the offset of digest,
        // the key->offset cannot be used as the offset of digest.
        int calc_valid_offset();
    };


class c1s1_strategy
{
public:
    c1s1_strategy(){}
    virtual ~c1s1_strategy(){}

    virtual int parse(char* _c1s1, int size) = 0;
    virtual int c1_validate_digest(c1s1* owner, bool& is_valid);
    int s1_validate_digest(c1s1* owner, bool& is_valid);
    virtual int calc_c1_digest(c1s1* owner, char*& c1_digest);
    int calc_s1_digest(c1s1* owner, char*& s1_digest);
    void copy_time_version(streamworker* stream, c1s1* owner);
    void copy_key(streamworker* stream);
    void copy_digest(streamworker* stream, bool with_digest);
    virtual int copy_to(c1s1* owner, char* bytes, int size, bool with_digest) = 0;
    virtual Hschem schema() = 0;
    int s1_create(c1s1* owner, c1s1* c1);
    char* get_key();
    char* get_digest();
    int dump(c1s1* owner, char* _c1s1, int size);
protected:
    key_block key;
    digest_block digest;
};


class SrsDH
    {
    private:
        DH* pdh;
    public:
        SrsDH();
        virtual ~SrsDH();
    public:
        /**
        * initialize dh, generate the public and private key.
        * @param ensure_128bytes_public_key whether ensure public key is 128bytes,
        *       sometimes openssl generate 127bytes public key.
        *       default to false to donot ensure.
        */
        virtual int initialize(bool ensure_128bytes_public_key = false);
        /**
        * copy the public key.
        * @param pkey the bytes to copy the public key.
        * @param pkey_size the max public key size, output the actual public key size.
        *       user should never ignore this size.
        * @remark, when ensure_128bytes_public_key, the size always 128.
        */
        virtual int copy_public_key(char* pkey, int32_t& pkey_size);
        /**
        * generate and copy the shared key.
        * generate the shared key with peer public key.
        * @param ppkey peer public key.
        * @param ppkey_size the size of ppkey.
        * @param skey the computed shared key.
        * @param skey_size the max shared key size, output the actual shared key size.
        *       user should never ignore this size.
        */
        virtual int copy_shared_key(const char* ppkey, int32_t ppkey_size, char* skey, int32_t& skey_size);
    private:
        virtual int do_initialize();
    };



class c1s1_strategy_schema0 : public c1s1_strategy
{
public:
    c1s1_strategy_schema0();
    ~c1s1_strategy_schema0();

    int parse(char* _c1s1, int size);
    int copy_to(c1s1* owner, char* bytes, int size, bool with_digest);
    Hschem schema(){
        return HAND_SHAKE_SCHEMA0;
    }
};


class c1s1_strategy_schema1 : public c1s1_strategy
{
public:
    c1s1_strategy_schema1();
    ~c1s1_strategy_schema1();

    int parse(char* _c1s1, int size);
    int copy_to(c1s1* owner, char* bytes, int size, bool with_digest);

    Hschem schema(){
        return HAND_SHAKE_SCHEMA1;
    }
};

class c1s1
{
public:
    c1s1();
    ~c1s1();

    int parse(char* _c1s1, int size, Hschem schema);
    int c1_validate_digest(bool& is_valid);
    int s1_validate_digest(bool& is_valid);
    Hschem schema();

    int s1_create(c1s1* c1);
    char* get_key();
    char* get_digest();
    int dump(char* _c1s1, int size);
public:
     // 4bytes
     int time;
     // 4bytes
     int version;
     // 764bytes+764bytes
     c1s1_strategy* payload;
};

    class c2s2
    {
    public:
        char random[1504];
        char digest[32];
    public:
        c2s2();
        virtual ~c2s2();
    public:
        /**
        * copy to bytes.
        * @param size, must always be 1536.
        */
        virtual int dump(char* _c2s2, int size);
        /**
        * parse the c2s2
        * @param size, must always be 1536.
        */
        virtual int parse(char* _c2s2, int size);
    public:
        /**
        * create c2.
        * random fill c2s2 1536 bytes
        * 
        * // client generate C2, or server valid C2
        * temp-key = HMACsha256(s1-digest, FPKey, 62)
        * c2-digest-data = HMACsha256(c2-random-data, temp-key, 32)
        */
        virtual int c2_create(c1s1* s1);
        
        /**
        * validate the c2 from client.
        */
        virtual int c2_validate(c1s1* s1, bool& is_valid);
    public:
        /**
        * create s2.
        * random fill c2s2 1536 bytes
        * 
        * // server generate S2, or client valid S2
        * temp-key = HMACsha256(c1-digest, FMSKey, 68)
        * s2-digest-data = HMACsha256(s2-random-data, temp-key, 32)
        */
        virtual int s2_create(c1s1* c1);
        
        /**
        * validate the s2 from server.
        */
        virtual int s2_validate(c1s1* c1, bool& is_valid);
    };


#endif