#include "rtmpcomplexhandshake.h"
#include "streamworker.h"
#include "utilityfunc.h"
#include "rtmpcore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static unsigned char URS_GenuineFMSKey[] = {
	0x47, 0x65, 0x6e, 0x75, 0x69, 0x6e, 0x65, 0x20,
	0x41, 0x64, 0x6f, 0x62, 0x65, 0x20, 0x46, 0x6c,
	0x61, 0x73, 0x68, 0x20, 0x4d, 0x65, 0x64, 0x69,
	0x61, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72,
	0x20, 0x30, 0x30, 0x31, // Genuine Adobe Flash Media Server 001
	0xf0, 0xee, 0xc2, 0x4a, 0x80, 0x68, 0xbe, 0xe8,
	0x2e, 0x00, 0xd0, 0xd1, 0x02, 0x9e, 0x7e, 0x57,
	0x6e, 0xec, 0x5d, 0x2d, 0x29, 0x80, 0x6f, 0xab,
	0x93, 0xb8, 0xe6, 0x36, 0xcf, 0xeb, 0x31, 0xae
}; // 68

// 62bytes FP key which is used to sign the client packet.
static unsigned char URS_GenuineFPKey[] = {
	0x47, 0x65, 0x6E, 0x75, 0x69, 0x6E, 0x65, 0x20,
	0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x46, 0x6C,
	0x61, 0x73, 0x68, 0x20, 0x50, 0x6C, 0x61, 0x79,
	0x65, 0x72, 0x20, 0x30, 0x30, 0x31, // Genuine Adobe Flash Player 001
	0xF0, 0xEE, 0xC2, 0x4A, 0x80, 0x68, 0xBE, 0xE8,
	0x2E, 0x00, 0xD0, 0xD1, 0x02, 0x9E, 0x7E, 0x57,
	0x6E, 0xEC, 0x5D, 0x2D, 0x29, 0x80, 0x6F, 0xAB,
	0x93, 0xB8, 0xE6, 0x36, 0xCF, 0xEB, 0x31, 0xAE
}; // 62

#define RFC2409_PRIME_1024 \
            "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1" \
            "29024E088A67CC74020BBEA63B139B22514A08798E3404DD" \
            "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245" \
            "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED" \
            "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381" \
            "FFFFFFFFFFFFFFFF"

static int do_openssl_HMACsha256(HMAC_CTX* ctx, const void* data, int data_size, void* digest, unsigned int* digest_size)
{
	int ret = 0;

	if (HMAC_Update(ctx, (unsigned char *)data, data_size) < 0) {
		ret = -1;
		return ret;
	}

	if (HMAC_Final(ctx, (unsigned char *)digest, digest_size) < 0) {
		ret = -1;
		return ret;
	}

	return ret;
}


static int openssl_HMACsha256(const void* key, int key_size, const void* data, int data_size, void* digest)
{
	int ret = 0;

	unsigned int digest_size = 0;

	unsigned char* temp_key = (unsigned char*)key;
	unsigned char* temp_digest = (unsigned char*)digest;

	if (key == NULL) {
		// use data to digest.
		// @see ./crypto/sha/sha256t.c
		// @see ./crypto/evp/digest.c
		if (EVP_Digest(data, data_size, temp_digest, &digest_size, EVP_sha256(), NULL) < 0)
		{
			ret = -1;
			return ret;
		}
	}
	else {
		// use key-data to digest.
		HMAC_CTX ctx;

		// @remark, if no key, use EVP_Digest to digest,
		// for instance, in python, hashlib.sha256(data).digest().
		HMAC_CTX_init(&ctx);
		if (HMAC_Init_ex(&ctx, temp_key, key_size, EVP_sha256(), NULL) < 0) {
			ret = -1;
			return ret;
		}

		ret = do_openssl_HMACsha256(&ctx, data, data_size, temp_digest, &digest_size);
		HMAC_CTX_cleanup(&ctx);

		if (ret != 0) {
			return ret;
		}
	}

	if (digest_size != 32) {
		ret = -1;
		return ret;
	}

	return ret;
}

key_block::key_block()
{
	offset = (int)rand();
	random0 = NULL;
	random1 = NULL;

	int valid_offset = calc_valid_offset();

	random0_size = valid_offset;

	if (random0_size > 0) {
		random0 = new char[random0_size];
		random_generate(random0, random0_size);
		// snprintf(random0, random0_size, "%s", RTMP_SIG_SRS_HANDSHAKE);
	}

	random_generate(key, sizeof(key));

	random1_size = 764 - valid_offset - 128 - 4;
	if (random1_size > 0) {
		random1 = new char[random1_size];
		random_generate(random1, random1_size);
		//  snprintf(random1, random1_size, "%s", RTMP_SIG_SRS_HANDSHAKE);
	}

}

key_block::~key_block()
{

}

int key_block::parse(streamworker* stream)
{
	int ret = 0;

	// read the last offset first, 760-763
	stream->streamback(764 - sizeof(int));
	offset = stream->read_4bytes();

	// reset stream to read others.
	stream->streamback(-764);

	int valid_offset = calc_valid_offset();

	random0_size = valid_offset;
	if (random0_size > 0) {
		random0 = new char[random0_size];
		stream->read_bytes(random0, random0_size);
	}

	stream->read_bytes(key, 128);

	random1_size = 764 - valid_offset - 128 - 4;
	if (random1_size > 0) {
		random1 = new char[random1_size];
		stream->read_bytes(random1, random1_size);
	}

	return ret;
}

int key_block::calc_valid_offset()
{
	int max_offset_size = 764 - 128 - 4;

	int valid_offset = 0;
	unsigned char* pp = (unsigned char*)&offset;
	valid_offset += *pp++;
	valid_offset += *pp++;
	valid_offset += *pp++;
	valid_offset += *pp++;

	return valid_offset % max_offset_size;
}


digest_block::digest_block()
{
	offset = (int)rand();
	random0 = NULL;
	random1 = NULL;

	int valid_offset = calc_valid_offset();

	random0_size = valid_offset;
	if (random0_size > 0) {
		random0 = new char[random0_size];
		random_generate(random0, random0_size);
		//snprintf(random0, random0_size, "%s", RTMP_SIG_SRS_HANDSHAKE);
	}

	random_generate(digest, sizeof(digest));

	random1_size = 764 - 4 - valid_offset - 32;
	if (random1_size > 0) {
		random1 = new char[random1_size];
		random_generate(random1, random1_size);
		// snprintf(random1, random1_size, "%s", RTMP_SIG_SRS_HANDSHAKE);
	}
}

digest_block::~digest_block()
{

}

int digest_block::parse(streamworker* stream)
{
	int ret = 0;

	// the digest must be 764 bytes.

	offset = stream->read_4bytes();

	int valid_offset = calc_valid_offset();

	random0_size = valid_offset;
	if (random0_size > 0) {
		random0 = new char[random0_size];
		stream->read_bytes(random0, random0_size);
	}

	stream->read_bytes(digest, 32);

	random1_size = 764 - 4 - valid_offset - 32;
	if (random1_size > 0) {
		random1 = new char[random1_size];
		stream->read_bytes(random1, random1_size);
	}

	return ret;
}

int digest_block::calc_valid_offset()
{
	int max_offset_size = 764 - 32 - 4;

	int valid_offset = 0;
	u_int8_t* pp = (u_int8_t*)&offset;
	valid_offset += *pp++;
	valid_offset += *pp++;
	valid_offset += *pp++;
	valid_offset += *pp++;

	return valid_offset % max_offset_size;
}



c1s1::c1s1()
{

}

c1s1::~c1s1()
{

}

int c1s1::parse(char* _c1s1, int size, Hschem schema)
{
	int ret = 0;

	if (schema != HAND_SHAKE_SCHEMA0 && schema != HAND_SHAKE_SCHEMA1) {
		ret = -1;
		printf("parse c1 failed. invalid schema=%d, ret=%d", schema, ret);
		return ret;
	}

	streamworker stream;
	stream.init(_c1s1, size);

	time = stream.read_4bytes();
	version = stream.read_4bytes(); // client c1 version

	if (schema == HAND_SHAKE_SCHEMA0) {
		payload = new c1s1_strategy_schema0();
	}
	else {
		payload = new c1s1_strategy_schema1();
	}

	return payload->parse(_c1s1, size);
}

int c1s1::s1_validate_digest(bool& is_valid)
{
	is_valid = false;
	assert(payload);
	return payload->s1_validate_digest(this, is_valid);
}


Hschem c1s1::schema()
{
	return payload->schema();
}

int c1s1::s1_create(c1s1* c1)
{
	int ret = 0;

	if (c1->schema() != HAND_SHAKE_SCHEMA0 && c1->schema() != HAND_SHAKE_SCHEMA1) {
		ret = -1;
		printf("create s1 failed. invalid schema=%d, ret=%d", c1->schema(), ret);
		return ret;
	}

	time = ::time(NULL);
	version = 0x01000504; // server s1 version


	if (c1->schema() == HAND_SHAKE_SCHEMA0) {
		payload = new c1s1_strategy_schema0();
	}
	else {
		payload = new c1s1_strategy_schema1();
	}

	return payload->s1_create(this, c1);
}

char* c1s1::get_key()
{
	assert(payload != NULL);
	return payload->get_key();
}


char* c1s1::get_digest()
{
	assert(payload != NULL);
	return payload->get_digest();
}

int c1s1::dump(char* _c1s1, int size)
{
	assert(size == 1536);
	assert(payload != NULL);
	return payload->dump(this, _c1s1, size);
}


int c1s1::c1_validate_digest(bool& is_valid)
{
	is_valid = false;
	assert(payload);
	return payload->c1_validate_digest(this, is_valid);
}

int c1s1_strategy::calc_s1_digest(c1s1* owner, char*& s1_digest)
{
	int ret = 0;

	/**
	* c1s1 is splited by digest:
	*     c1s1-part1: n bytes (time, version, key and digest-part1).
	*     digest-data: 32bytes
	*     c1s1-part2: (1536-n-32)bytes (digest-part2)
	* @return a new allocated bytes, user must free it.
	*/
	char* c1s1_joined_bytes = new char[1536 - 32];

	if ((ret = copy_to(owner, c1s1_joined_bytes, 1536 - 32, false)) != 0) {
		return ret;
	}

	s1_digest = new char[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(URS_GenuineFPKey, 36, c1s1_joined_bytes, 1536 - 32, s1_digest)) != 0) {
		printf("calc digest for s1 failed. ret=%d\n", ret);
		return ret;
	}
	printf("digest calculated for s1\n");

	return ret;
}

int c1s1_strategy::s1_validate_digest(c1s1* owner, bool& is_valid)
{
	int ret = 0;

	char* s1_digest = NULL;

	if ((ret = calc_s1_digest(owner, s1_digest)) != 0) {
		printf("validate s1 error, failed to calc digest. ret=%d", ret);
		return ret;
	}

	assert(s1_digest != NULL);


	is_valid = bytes_equals(digest.digest, s1_digest, 32);

	return ret;
}

int c1s1_strategy::dump(c1s1* owner, char* _c1s1, int size)
{
	assert(size == 1536);
	return copy_to(owner, _c1s1, size, true);
}

int c1s1_strategy::s1_create(c1s1* owner, c1s1* c1)
{
	int ret = 0;

	SrsDH dh;

	// ensure generate 128bytes public key.
	if ((ret = dh.initialize(true)) != 0) {
		return ret;
	}

	// directly generate the public key.
	// @see: https://github.com/ossrs/srs/issues/148
	int pkey_size = 128;
	if ((ret = dh.copy_shared_key(c1->get_key(), 128, key.key, pkey_size)) != 0) {
		printf("calc s1 key failed. ret=%d", ret);
		return ret;
	}

	// altough the public key is always 128bytes, but the share key maybe not.
	// we just ignore the actual key size, but if need to use the key, must use the actual size.
	// TODO: FIXME: use the actual key size.
	//srs_assert(pkey_size == 128);
	printf("calc s1 key success.\n");

	char* s1_digest = NULL;
	if ((ret = calc_s1_digest(owner, s1_digest)) != 0) {
		printf("calc s1 digest failed. ret=%d\n", ret);
		return ret;
	}
	printf("calc s1 digest success.\n");

	assert(s1_digest != NULL);

	memcpy(digest.digest, s1_digest, 32);
	printf("copy s1 key success.");

	return ret;
}


int c1s1_strategy::c1_validate_digest(c1s1* owner, bool& is_valid)
{
	int ret = 0;

	char* c1_digest = NULL;

	if ((ret = calc_c1_digest(owner, c1_digest)) != 0) {
		printf("validate c1 error, failed to calc digest. ret=%d", ret);
		return ret;
	}

	is_valid = bytes_equals(digest.digest, c1_digest, 32);

	return ret;
}

char* c1s1_strategy::get_key()
{
	return key.key;
}

char* c1s1_strategy::get_digest()
{
	return digest.digest;
}

int c1s1_strategy::calc_c1_digest(c1s1* owner, char*& c1_digest)
{
	int ret = 0;

	/**
	* c1s1 is splited by digest:
	*     c1s1-part1: n bytes (time, version, key and digest-part1).
	*     digest-data: 32bytes
	*     c1s1-part2: (1536-n-32)bytes (digest-part2)
	* @return a new allocated bytes, user must free it.
	*/
	char* c1s1_joined_bytes = new char[1536 - 32];
	memset(c1s1_joined_bytes, 0, 1536 - 32);
	if ((ret = copy_to(owner, c1s1_joined_bytes, 1536 - 32, false)) != 0) {
		return ret;
	}

	c1_digest = new char[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(URS_GenuineFPKey, 30, c1s1_joined_bytes, 1536 - 32, c1_digest)) != 0) {
		printf("calc digest for c1 failed. ret=%d\n", ret);
		return ret;
	}
	printf("digest calculated for c1\n");

	return ret;
}

void c1s1_strategy::copy_time_version(streamworker* stream, c1s1* owner)
{
	stream->write4bytes(owner->time);

	stream->write4bytes(owner->version);
}

void c1s1_strategy::copy_key(streamworker* stream)
{
	int total = key.random0_size + 128 + key.random1_size + 4;

	// 764bytes key block
	if (key.random0_size > 0) {
		stream->write_stream(key.random0, key.random0_size);
	}

	stream->write_stream(key.key, 128);

	if (key.random1_size > 0) {
		stream->write_stream(key.random1, key.random1_size);
	}

	stream->write4bytes(key.offset);
}

void c1s1_strategy::copy_digest(streamworker* stream, bool with_digest)
{
	assert(key.random0_size >= 0);
	assert(key.random1_size >= 0);

	int total = 4 + digest.random0_size + digest.random1_size;
	if (with_digest) {
		total += 32;
	}

	// 732bytes digest block without the 32bytes digest-data
	// nbytes digest block part1
	stream->write4bytes(digest.offset);

	// digest random padding.
	if (digest.random0_size > 0) {
		stream->write_stream(digest.random0, digest.random0_size);
	}

	// digest
	if (with_digest) {
		stream->write_stream(digest.digest, 32);
	}

	// nbytes digest block part2
	if (digest.random1_size > 0) {
		stream->write_stream(digest.random1, digest.random1_size);
	}
}

c1s1_strategy_schema1::c1s1_strategy_schema1()
{

}

c1s1_strategy_schema1::~c1s1_strategy_schema1()
{

}

int c1s1_strategy_schema1::parse(char* _c1s1, int size)
{
	int ret = 0;

	assert(size == 1536);

	streamworker stream;

	stream.init(_c1s1 + 8, 764);

	if ((ret = digest.parse(&stream)) != 0) {
		printf("parse the c1 digest failed. ret=%d", ret);
		return ret;
	}

	stream.init(_c1s1 + 8 + 764, 764);

	if ((ret = key.parse(&stream)) != 0) {
		printf("parse the c1 key failed. ret=%d", ret);
		return ret;
	}

	printf("parse c1 digest-key success\n");

	return ret;
}

int c1s1_strategy_schema1::copy_to(c1s1* owner, char* bytes, int size, bool with_digest)
{
	int ret = 0;

	if (with_digest) {
		assert(size == 1536);
	}
	else {
		assert(size == 1504);
	}

	streamworker stream;

	stream.init(bytes, size);

	copy_time_version(&stream, owner);
	copy_digest(&stream, with_digest);
	copy_key(&stream);

	return ret;
}

c1s1_strategy_schema0::c1s1_strategy_schema0()
{

}

c1s1_strategy_schema0::~c1s1_strategy_schema0()
{

}

int c1s1_strategy_schema0::copy_to(c1s1* owner, char* bytes, int size, bool with_digest)
{
	int ret = 0;

	if (with_digest) {
		assert(size == 1536);
	}
	else {
		assert(size == 1504);
	}

	streamworker stream;

	stream.init(bytes, size);

	copy_time_version(&stream, owner);
	copy_key(&stream);
	copy_digest(&stream, with_digest);

	return ret;
}

int c1s1_strategy_schema0::parse(char* _c1s1, int size)
{
	int ret = 0;

	streamworker stream;
	stream.init(_c1s1 + 8, 764);

	if ((ret = key.parse(&stream)) != 0) {
		printf("parse the c1 key failed. ret=%d", ret);
		return ret;
	}

	stream.init(_c1s1 + 8 + 764, 764);

	if ((ret = digest.parse(&stream)) != 0) {
		printf("parse the c1 digest failed. ret=%d", ret);
		return ret;
	}

	printf("parse c1 key-digest success");

	return ret;
}

SrsDH::SrsDH()
{
	pdh = NULL;
}

SrsDH::~SrsDH()
{
	if (pdh != NULL) {
		if (pdh->p != NULL) {
			BN_free(pdh->p);
			pdh->p = NULL;
		}
		if (pdh->g != NULL) {
			BN_free(pdh->g);
			pdh->g = NULL;
		}
		DH_free(pdh);
		pdh = NULL;
	}
}

int SrsDH::initialize(bool ensure_128bytes_public_key)
{
	int ret = 0;

	for (;;) {
		if ((ret = do_initialize()) != 0) {
			return ret;
		}

		if (ensure_128bytes_public_key) {
			int key_size = BN_num_bytes(pdh->pub_key);
			if (key_size != 128) {
				printf("regenerate 128B key, current=%dB\n", key_size);
				continue;
			}
		}

		break;
	}

	return ret;
}

int SrsDH::copy_public_key(char* pkey, int32_t& pkey_size)
{
	int ret = 0;

	// copy public key to bytes.
	// sometimes, the key_size is 127, seems ok.
	int key_size = BN_num_bytes(pdh->pub_key);
	assert(key_size > 0);

	// maybe the key_size is 127, but dh will write all 128bytes pkey,
	// so, donot need to set/initialize the pkey.
	// @see https://github.com/ossrs/srs/issues/165
	key_size = BN_bn2bin(pdh->pub_key, (unsigned char*)pkey);
	assert(key_size > 0);

	// output the size of public key.
	// @see https://github.com/ossrs/srs/issues/165
	assert(key_size <= pkey_size);
	pkey_size = key_size;

	return ret;
}

int SrsDH::copy_shared_key(const char* ppkey, int32_t ppkey_size, char* skey, int32_t& skey_size)
{
	int ret = 0;

	BIGNUM* ppk = NULL;
	if ((ppk = BN_bin2bn((const unsigned char*)ppkey, ppkey_size, 0)) == NULL) {
		ret = -1;
		return ret;
	}

	// if failed, donot return, do cleanup, @see ./test/dhtest.c:168
	// maybe the key_size is 127, but dh will write all 128bytes skey,
	// so, donot need to set/initialize the skey.
	// @see https://github.com/ossrs/srs/issues/165
	int key_size = DH_compute_key((unsigned char*)skey, ppk, pdh);

	if (key_size < ppkey_size) {
		printf("shared key size=%d, ppk_size=%d\n", key_size, ppkey_size);
	}

	if (key_size < 0 || key_size > skey_size) {
		ret = -1;
	}
	else {
		skey_size = key_size;
	}

	if (ppk) {
		BN_free(ppk);
	}

	return ret;
}

int SrsDH::do_initialize()
{
	int ret = 0;

	int32_t bits_count = 1024;

	//1. Create the DH
	if ((pdh = DH_new()) == NULL) {
		ret = -1;
		return ret;
	}

	//2. Create his internal p and g
	if ((pdh->p = BN_new()) == NULL) {
		ret = -1;
		return ret;
	}
	if ((pdh->g = BN_new()) == NULL) {
		ret = -1;
		return ret;
	}

	//3. initialize p and g, @see ./test/ectest.c:260
	if (!BN_hex2bn(&pdh->p, RFC2409_PRIME_1024)) {
		ret = -1;
		return ret;
	}
	// @see ./test/bntest.c:1764
	if (!BN_set_word(pdh->g, 2)) {
		ret = -1;
		return ret;
	}

	// 4. Set the key length
	pdh->length = bits_count;

	// 5. Generate private and public key
	// @see ./test/dhtest.c:152
	if (!DH_generate_key(pdh)) {
		ret = -1;
		return ret;
	}

	return ret;
}


c2s2::c2s2()
{
	random_generate(random, 1504);

	int size = snprintf(random, 1504, "%s", RTMP_SIG_SRS_HANDSHAKE);
	assert(++size < 1504);
	snprintf(random + 1504 - size, size, "%s", RTMP_SIG_SRS_HANDSHAKE);

	random_generate(digest, 32);
}

c2s2::~c2s2()
{
}


int c2s2::dump(char* _c2s2, int size)
{
	assert(size == 1536);

	memcpy(_c2s2, random, 1504);
	memcpy(_c2s2 + 1504, digest, 32);

	return 0;
}

int c2s2::parse(char* _c2s2, int size)
{
	assert(size == 1536);

	memcpy(random, _c2s2, 1504);
	memcpy(digest, _c2s2 + 1504, 32);

	return 0;
}

int c2s2::c2_create(c1s1* s1)
{
	int ret = 0;

	char temp_key[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(URS_GenuineFPKey, 62, s1->get_digest(), 32, temp_key)) != 0) {
		printf("create c2 temp key failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate c2 temp key success.\n");

	char _digest[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(temp_key, 32, random, 1504, _digest)) != 0) {
		printf("create c2 digest failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate c2 digest success.\n");

	memcpy(digest, _digest, 32);

	return ret;
}

int c2s2::c2_validate(c1s1* s1, bool& is_valid)
{
	is_valid = false;
	int ret = 0;

	char temp_key[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(URS_GenuineFPKey, 62, s1->get_digest(), 32, temp_key)) != 0) {
		printf("create c2 temp key failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate c2 temp key success.\n");

	char _digest[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(temp_key, 32, random, 1504, _digest)) != 0) {
		printf("create c2 digest failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate c2 digest success.\n");

	is_valid = bytes_equals(digest, _digest, 32);

	return ret;
}

int c2s2::s2_create(c1s1* c1)
{
	int ret = 0;

	char temp_key[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(URS_GenuineFMSKey, 68, c1->get_digest(), 32, temp_key)) != 0) {
		printf("create s2 temp key failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate s2 temp key success.\n");

	char _digest[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(temp_key, 32, random, 1504, _digest)) != 0) {
		printf("create s2 digest failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate s2 digest success.\n");

	memcpy(digest, _digest, 32);

	return ret;
}

int c2s2::s2_validate(c1s1* c1, bool& is_valid)
{
	is_valid = false;
	int ret = 0;

	char temp_key[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(URS_GenuineFMSKey, 68, c1->get_digest(), 32, temp_key)) != 0) {
		printf("create s2 temp key failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate s2 temp key success.\n");

	char _digest[OPENSSL_HASH_SIZE];
	if ((ret = openssl_HMACsha256(temp_key, 32, random, 1504, _digest)) != 0) {
		printf("create s2 digest failed. ret=%d\n", ret);
		return ret;
	}
	printf("generate s2 digest success.\n");

	is_valid = bytes_equals(digest, _digest, 32);

	return ret;
}