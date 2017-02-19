#ifndef TS_MUXER_H
#define TS_MUXER_H
#include "avccodec.h"
#include "streamworker.h"
#include <stdio.h>
#define TS_PACKET_SIZE 184
#define TS_SYNC_BYTE  0x47
#define TS_PMT_NUMBER 1
#define TS_PMT_PID 0x1001
#define TS_VIDEO_AVC_PID 0x100
#define TS_AUDIO_AAC_PID 0x101
#define TS_VIDEO_PES_STREAMID 0xE0
#define TS_AUDIO_PES_STREAMID 0xC0

#define TS_PMT_NUMBER 1    //目前只支持一个pmt表
#define TS_PMT_PID 0x1001

enum TsPESStreamId
{
	// program_stream_map
	TsPESStreamIdProgramStreamMap = 0xbc, // 0b10111100
	// private_stream_1
	TsPESStreamIdPrivateStream1 = 0xbd, // 0b10111101
	// padding_stream
	TsPESStreamIdPaddingStream = 0xbe, // 0b10111110
	// private_stream_2
	TsPESStreamIdPrivateStream2 = 0xbf, // 0b10111111

	// 110x xxxx
	// ISO/IEC 13818-3 or ISO/IEC 11172-3 or ISO/IEC 13818-7 or ISO/IEC
	// 14496-3 audio stream number x xxxx
	// ((sid >> 5) & 0x07) == SrsTsPESStreamIdAudio
	// @remark, use SrsTsPESStreamIdAudioCommon as actually audio, SrsTsPESStreamIdAudio to check whether audio.
	TsPESStreamIdAudioChecker = 0x06, // 0b110
	TsPESStreamIdAudioCommon = 0xc0,

	// 1110 xxxx
	// ITU-T Rec. H.262 | ISO/IEC 13818-2 or ISO/IEC 11172-2 or ISO/IEC
	// 14496-2 video stream number xxxx
	// ((stream_id >> 4) & 0x0f) == SrsTsPESStreamIdVideo
	// @remark, use SrsTsPESStreamIdVideoCommon as actually video, SrsTsPESStreamIdVideo to check whether video.
	TsPESStreamIdVideoChecker = 0x0e, // 0b1110
	TsPESStreamIdVideoCommon = 0xe0,

	// ECM_stream
	TsPESStreamIdEcmStream = 0xf0, // 0b11110000
	// EMM_stream
	TsPESStreamIdEmmStream = 0xf1, // 0b11110001
	// DSMCC_stream
	TsPESStreamIdDsmccStream = 0xf2, // 0b11110010
	// 13522_stream
	TsPESStreamId13522Stream = 0xf3, // 0b11110011
	// H_222_1_type_A
	TsPESStreamIdH2221TypeA = 0xf4, // 0b11110100
	// H_222_1_type_B
	TsPESStreamIdH2221TypeB = 0xf5, // 0b11110101
	// H_222_1_type_C
	TsPESStreamIdH2221TypeC = 0xf6, // 0b11110110
	// H_222_1_type_D
	TsPESStreamIdH2221TypeD = 0xf7, // 0b11110111
	// H_222_1_type_E
	TsPESStreamIdH2221TypeE = 0xf8, // 0b11111000
	// ancillary_stream
	TsPESStreamIdAncillaryStream = 0xf9, // 0b11111001
	// SL_packetized_stream
	TsPESStreamIdSlPacketizedStream = 0xfa, // 0b11111010
	// FlexMux_stream
	TsPESStreamIdFlexMuxStream = 0xfb, // 0b11111011
	// reserved data stream
	// 1111 1100 ... 1111 1110
	// program_stream_directory
	TsPESStreamIdProgramStreamDirectory = 0xff, // 0b11111111
};

class tspacket;

enum TsPid
{
	// Program Association Table(see Table 2-25).
	TsPidPAT = 0x00,
	// Conditional Access Table (see Table 2-27).
	TsPidCAT = 0x01,
	// Transport Stream Description Table
	TsPidTSDT = 0x02,
	// Reserved
	TsPidReservedStart = 0x03,
	TsPidReservedEnd = 0x0f,
	// May be assigned as network_PID, Program_map_PID, elementary_PID, or for other purposes
	TsPidAppStart = 0x10,
	TsPidAppEnd = 0x1ffe,
	// null packets (see Table 2-3)
	TsPidNULL = 0x01FFF,
};

/**
* 2.4.4.4 Table_id assignments, hls-mpeg-ts-iso13818-1.pdf, page 62
* The table_id field identifies the contents of a Transport Stream PSI section as shown in Table 2-26.
*/
enum TsPsiId
{
	// program_association_section
	TsPsiIdPas = 0x00,
	// conditional_access_section (CA_section)
	TsPsiIdCas = 0x01,
	// TS_program_map_section
	TsPsiIdPms = 0x02,
	// TS_description_section
	TsPsiIdDs = 0x03,
	// ISO_IEC_14496_scene_description_section
	TsPsiIdSds = 0x04,
	// ISO_IEC_14496_object_descriptor_section
	TsPsiIdOds = 0x05,
	// ITU-T Rec. H.222.0 | ISO/IEC 13818-1 reserved
	TsPsiIdIso138181Start = 0x06,
	TsPsiIdIso138181End = 0x37,
	// Defined in ISO/IEC 13818-6
	TsPsiIdIso138186Start = 0x38,
	TsPsiIdIso138186End = 0x3F,
	// User private
	TsPsiIdUserStart = 0x40,
	SrsTsPsiIdUserEnd = 0xFE,
	// forbidden
	TsPsiIdForbidden = 0xFF,
};

enum TsStream
{
	// ITU-T | ISO/IEC Reserved
	TsStreamReserved = 0x00,
	// ISO/IEC 11172 Video
	// ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream
	// ISO/IEC 11172 Audio
	// ISO/IEC 13818-3 Audio
	TsStreamAudioMp3 = 0x04,
	// ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections
	// ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data
	// ISO/IEC 13522 MHEG
	// ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC
	// ITU-T Rec. H.222.1
	// ISO/IEC 13818-6 type A
	// ISO/IEC 13818-6 type B
	// ISO/IEC 13818-6 type C
	// ISO/IEC 13818-6 type D
	// ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary
	// ISO/IEC 13818-7 Audio with ADTS transport syntax
	TsStreamAudioAAC = 0x0f,
	// ISO/IEC 14496-2 Visual
	TsStreamVideoMpeg4 = 0x10,
	// ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1
	TsStreamAudioMpeg4 = 0x11,
	// ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets
	// ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections.
	// ISO/IEC 13818-6 Synchronized Download Protocol
	// ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
	// 0x15-0x7F
	TsStreamVideoH264 = 0x1b,
	// User Private
	// 0x80-0xFF
	TsStreamAudioAC3 = 0x81,
	TsStreamAudioDTS = 0x8a,
};

/**
* the transport_scrambling_control of ts packet,
* Table 2-4 - Scrambling control values, hls-mpeg-ts-iso13818-1.pdf, page 38
*/
enum TsScrambled
{
	// Not scrambled
	TsScrambledDisabled = 0x00,
	// User-defined
	TsScrambledUserDefined1 = 0x01,
	// User-defined
	TsScrambledUserDefined2 = 0x02,
	// User-defined
	TsScrambledUserDefined3 = 0x03,
};

/**
* the adaption_field_control of ts packet,
* Table 2-5 - Adaptation field control values, hls-mpeg-ts-iso13818-1.pdf, page 38
*/
enum TsAdaptationFieldType
{
	// Reserved for future use by ISO/IEC
	TsAdaptationFieldTypeReserved = 0x00,
	// No adaptation_field, payload only
	TsAdaptationFieldTypePayloadOnly = 0x01,
	// Adaptation_field only, no payload
	TsAdaptationFieldTypeAdaptionOnly = 0x02,
	// Adaptation_field followed by payload
	TsAdaptationFieldTypeBoth = 0x03,
};

struct TsAdaptationField
{

};

class tsplayload
{
public:
	tsplayload(tspacket* pkt);
	virtual ~tsplayload();
	virtual int size() = 0;
	virtual int encode(streamworker* stream) = 0;
protected:
	tspacket* m_packet;
};

class tspsiplayload : public tsplayload
{
public:
	tspsiplayload(tspacket* pkt);
	virtual ~tspsiplayload();
	int size();
	int encode(streamworker* stream);
	virtual int psi_size() = 0;
	virtual int psi_encode(streamworker *stream) = 0;
public:
	unsigned char pointer_field; //when pkt is psi packet , pointer_field must be 0 before playload
	unsigned char table_id;
	unsigned char section_syntax_indicator : 1;
	unsigned char const0_value : 1;   //must be 0
	unsigned char reserved : 2;
	unsigned int section_length;
	int CRC_32; //32bits
};

class tspatprogram
{
public:
	tspatprogram(int pmtnumber,int pmtpid);
	~tspatprogram();
	int size(){ return 4; }
	int encode(streamworker* stream);

public:
	// 4B
    /**
    * Program_number is a 16-bit field. It specifies the program to which the program_map_PID is
    * applicable. When set to 0x0000, then the following PID reference shall be the network PID. For all other cases the value
    * of this field is user defined. This field shall not take any single value more than once within one version of the Program
    * Association Table.
    */
    int number; // 16bits
    /**
    * reverved value, must be '1'
    */
    char const1_value:3; //3bits
    /**
    * program_map_PID/network_PID 13bits
    * network_PID - The network_PID is a 13-bit field, which is used only in conjunction with the value of the
    * program_number set to 0x0000, specifies the PID of the Transport Stream packets which shall contain the Network
    * Information Table. The value of the network_PID field is defined by the user, but shall only take values as specified in
    * Table 2-3. The presence of the network_PID is optional.
    */
    int pid; //13bits
};

class tsplayloadesinfo
{
public:
	tsplayloadesinfo();   //es stream type and elementary_PID
	~tsplayloadesinfo();

	int size();
	int encode(streamworker* stream);

public:
	// 1B
	/**
	* This is an 8-bit field specifying the type of program element carried within the packets with the PID
	* whose value is specified by the elementary_PID. The values of stream_type are specified in Table 2-29.
	*/
	TsStream stream_type; //8bits

	// 2B
	/**
	* reverved value, must be '1'
	*/
	char const1_value0:3; //3bits
	/**
	* This is a 13-bit field specifying the PID of the Transport Stream packets which carry the associated
	* program element.
	*/
	short elementary_PID:13; //13bits

	// (2+x)B
	/**
	* reverved value, must be '1'
	*/
	char const1_value1:4; //4bits
	/**
	* This is a 12-bit field, the first two bits of which shall be '00'. The remaining 10 bits specify the number
	* of bytes of the descriptors of the associated program element immediately following the ES_info_length field.
	*/
	short ES_info_length:12; //12bits
	char* ES_info; //[ES_info_length] bytes.
};


class tspmtplayload :public tspsiplayload
{
public:
	tspmtplayload(tspacket* pkt);
	~tspmtplayload();
	int psi_size();
	int psi_encode(streamworker* stream);
public:
	// 2B
	/**
	* program_number is a 16-bit field. It specifies the program to which the program_map_PID is
	* applicable. One program definition shall be carried within only one TS_program_map_section. This implies that a
	* program definition is never longer than 1016 (0x3F8). See Informative Annex C for ways to deal with the cases when
	* that length is not sufficient. The program_number may be used as a designation for a broadcast channel, for example. By
	* describing the different program elements belonging to a program, data from different sources (e.g. sequential events)
	* can be concatenated together to form a continuous set of streams using a program_number. For examples of applications
	* refer to Annex C.
	*/
	short program_number; //16bits

	// 1B
	/**
	* reverved value, must be '1'
	*/
	char const1_value0:2; //2bits
	/**
	* This 5-bit field is the version number of the TS_program_map_section. The version number shall be
	* incremented by 1 modulo 32 when a change in the information carried within the section occurs. Version number refers
	* to the definition of a single program, and therefore to a single section. When the current_next_indicator is set to '1', then
	* the version_number shall be that of the currently applicable TS_program_map_section. When the current_next_indicator
	* is set to '0', then the version_number shall be that of the next applicable TS_program_map_section.
	*/
	char version_number:5; //5bits
	/**
	* A 1-bit field, which when set to '1' indicates that the TS_program_map_section sent is
	* currently applicable. When the bit is set to '0', it indicates that the TS_program_map_section sent is not yet applicable
	* and shall be the next TS_program_map_section to become valid.
	*/
	char current_next_indicator:1; //1bit

	// 1B
	/**
	* The value of this 8-bit field shall be 0x00.
	*/
	unsigned char section_number; //8bits

	// 1B
	/**
	* The value of this 8-bit field shall be 0x00.
	*/
	unsigned char last_section_number; //8bits

	// 2B
	/**
	* reverved value, must be '1'
	*/
	char const1_value1:3; //3bits
	/**
	* This is a 13-bit field indicating the PID of the Transport Stream packets which shall contain the PCR fields
	* valid for the program specified by program_number. If no PCR is associated with a program definition for private
	* streams, then this field shall take the value of 0x1FFF. Refer to the semantic definition of PCR in 2.4.3.5 and Table 2-3
	* for restrictions on the choice of PCR_PID value.
	*/
	short PCR_PID:13; //13bits

	// 2B
	char const1_value2:4; //4bits
	/**
	* This is a 12-bit field, the first two bits of which shall be '00'. The remaining 10 bits specify the
	* number of bytes of the descriptors immediately following the program_info_length field.
	*/
	unsigned short  program_info_length:12; //12bits
	char* program_info_desc; //[program_info_length]bytes

	vector<tsplayloadesinfo*> infos;

};

class tspatplayload : public tspsiplayload
{
public:
	tspatplayload(tspacket* pkt);
	~tspatplayload();
	int psi_size();
	int psi_encode(streamworker* stream);
	
public:
	// 2B
	/**
	* This is a 16-bit field which serves as a label to identify this Transport Stream from any other
	* multiplex within a network. Its value is defined by the user.
	*/
	unsigned int transport_stream_id; //16bits

	// 1B
	/**
	* reverved value, must be '1'
	*/
	char const1_value:2; //2bits
	/**
	* This 5-bit field is the version number of the whole Program Association Table. The version number
	* shall be incremented by 1 modulo 32 whenever the definition of the Program Association Table changes. When the
	* current_next_indicator is set to '1', then the version_number shall be that of the currently applicable Program Association
	* Table. When the current_next_indicator is set to '0', then the version_number shall be that of the next applicable Program
	* Association Table.
	*/
	unsigned char version_number:5; //5bits
	/**
	* A 1-bit indicator, which when set to '1' indicates that the Program Association Table sent is
	* currently applicable. When the bit is set to '0', it indicates that the table sent is not yet applicable and shall be the next
	* table to become valid.
	*/
	char current_next_indicator:1; //1bit

	// 1B
	/**
	* This 8-bit field gives the number of this section. The section_number of the first section in the
	* Program Association Table shall be 0x00. It shall be incremented by 1 with each additional section in the Program
	* Association Table.
	*/
	unsigned char section_number; //8bits

	// 1B
	/**
	* This 8-bit field specifies the number of the last section (that is, the section with the highest
	* section_number) of the complete Program Association Table.
	*/
	unsigned char last_section_number; //8bits
	vector<tspatprogram*> programs;
};

class tspesplayload : public tsplayload
{
public:
	tspesplayload(tspacket* pkt);
	~tspesplayload();

	int size();
	int encode(streamworker* stream);

public:
	int packet_start_code_prefix;  //24bits
	int stream_id;    //for video  0xE0  ， for audio 0xC0
	short PES_packet_length; 
	char const2bits:2; //2bits  0x02
	char PES_scrambling_control:2;  //current only need 0x00;
	char PES_priority:1;     //0
	char data_alignment_indicator : 1; //0
	char copyright : 1; //0
	char original_or_copy : 1;//0
	/*
	This is a 2-bit field. When the PTS_DTS_flags field is set to '10', the PTS fields shall be present in
	the PES packet header. When the PTS_DTS_flags field is set to '11', both the PTS fields and DTS fields shall be present
	in the PES packet header. When the PTS_DTS_flags field is set to '00' no PTS or DTS fields shall be present in the PES
	packet header. The value '01' is forbidden.
	*/
	char PTS_DTS_flags : 2; 
	char ESCR_flag : 1;   //0
	char ES_rate_flag : 1; //0
	char DSM_trick_mode_flag : 1; //0
	char additional_copy_info_flag : 1;//0
	char PES_CRC_flag : 1;//0
	char PES_extension_flag : 1;//0
	int PES_header_data_length;   //从SRS源码看应该是PES_header_data_length字段之后的填充数据的大小

};

class tspacket{
public:
	tspacket();
	~tspacket();
public:
	// 1B
	/**
	* The sync_byte is a fixed 8-bit field whose value is '0100 0111' (0x47). Sync_byte emulation in the choice of
	* values for other regularly occurring fields, such as PID, should be avoided.
	*/
	unsigned char sync_byte; //8bits

	// 2B
	/**
	* The transport_error_indicator is a 1-bit flag. When set to '1' it indicates that at least
	* 1 uncorrectable bit error exists in the associated Transport Stream packet. This bit may be set to '1' by entities external to
	* the transport layer. When set to '1' this bit shall not be reset to '0' unless the bit value(s) in error have been corrected.
	*/
	unsigned char transport_error_indicator:1; //1bit
	/**
	* The payload_unit_start_indicator is a 1-bit flag which has normative meaning for
	* Transport Stream packets that carry PES packets (refer to 2.4.3.6) or PSI data (refer to 2.4.4).
	*
	* When the payload of the Transport Stream packet contains PES packet data, the payload_unit_start_indicator has the
	* following significance: a '1' indicates that the payload of this Transport Stream packet will commence(start) with the first byte
	* of a PES packet and a '0' indicates no PES packet shall start in this Transport Stream packet. If the
	* payload_unit_start_indicator is set to '1', then one and only one PES packet starts in this Transport Stream packet. This
	* also applies to private streams of stream_type 6 (refer to Table 2-29).
	*
	* When the payload of the Transport Stream packet contains PSI data, the payload_unit_start_indicator has the following
	* significance: if the Transport Stream packet carries the first byte of a PSI section, the payload_unit_start_indicator value
	* shall be '1', indicating that the first byte of the payload of this Transport Stream packet carries the pointer_field. If the
	* Transport Stream packet does not carry the first byte of a PSI section, the payload_unit_start_indicator value shall be '0',
	* indicating that there is no pointer_field in the payload. Refer to 2.4.4.1 and 2.4.4.2. This also applies to private streams of
	* stream_type 5 (refer to Table 2-29).
	*
	* For null packets the payload_unit_start_indicator shall be set to '0'.
	*
	* The meaning of this bit for Transport Stream packets carrying only private data is not defined in this Specification.
	*/
	unsigned char payload_unit_start_indicator:1; //1bit
	/**
	* The transport_priority is a 1-bit indicator. When set to '1' it indicates that the associated packet is
	* of greater priority than other packets having the same PID which do not have the bit set to '1'. The transport mechanism
	* can use this to prioritize its data within an elementary stream. Depending on the application the transport_priority field
	* may be coded regardless of the PID or within one PID only. This field may be changed by channel specific encoders or
	* decoders.
	*/
	unsigned char transport_priority:1; //1bit
	/**
	* The PID is a 13-bit field, indicating the type of the data stored in the packet payload. PID value 0x0000 is
	* reserved for the Program Association Table (see Table 2-25). PID value 0x0001 is reserved for the Conditional Access
	* Table (see Table 2-27). PID values 0x0002 - 0x000F are reserved. PID value 0x1FFF is reserved for null packets (see
	* Table 2-3).
	*/
	int pid:13; //13bits

	// 1B
	/**
	* This 2-bit field indicates the scrambling mode of the Transport Stream packet payload.
	* The Transport Stream packet header, and the adaptation field when present, shall not be scrambled. In the case of a null
	* packet the value of the transport_scrambling_control field shall be set to '00' (see Table 2-4).
	*/
	TsScrambled transport_scrambling_control; //2bits
	/**
	* This 2-bit field indicates whether this Transport Stream packet header is followed by an
	* adaptation field and/or payload (see Table 2-5).
	*
	* ITU-T Rec. H.222.0 | ISO/IEC 13818-1 decoders shall discard Transport Stream packets with the
	* adaptation_field_control field set to a value of '00'. In the case of a null packet the value of the adaptation_field_control
	* shall be set to '01'.
	*/
	TsAdaptationFieldType adaption_field_control; //2bits
	/**
	* The continuity_counter is a 4-bit field incrementing with each Transport Stream packet with the
	* same PID. The continuity_counter wraps around to 0 after its maximum value. The continuity_counter shall not be
	* incremented when the adaptation_field_control of the packet equals '00'(reseverd) or '10'(adaptation field only).
	*
	* In Transport Streams, duplicate packets may be sent as two, and only two, consecutive Transport Stream packets of the
	* same PID. The duplicate packets shall have the same continuity_counter value as the original packet and the
	* adaptation_field_control field shall be equal to '01'(payload only) or '11'(both). In duplicate packets each byte of the original packet shall be
	* duplicated, with the exception that in the program clock reference fields, if present, a valid value shall be encoded.
	*
	* The continuity_counter in a particular Transport Stream packet is continuous when it differs by a positive value of one
	* from the continuity_counter value in the previous Transport Stream packet of the same PID, or when either of the nonincrementing
	* conditions (adaptation_field_control set to '00' or '10', or duplicate packets as described above) are met.
	* The continuity counter may be discontinuous when the discontinuity_indicator is set to '1' (refer to 2.4.3.4). In the case of
	* a null packet the value of the continuity_counter is undefined.
	*/
	unsigned char continuity_counter:4; //4bits
	TsAdaptationField* adaptation_field;

	tsplayload* playload;

public:
	int size();
	int encode_packet_data(streamworker* stream);
};


class tssegment{
public:
	tssegment();
	~tssegment();
	int encode_pat_pmt();
	int write_data(tspacket* pkt);
	int write_direct_stream(tspacket* pkt,streamworker* stream);
	int close_tssegment();
	
private:
	int write_segment(char* buf, int length);
	int flush_file(streamworker* stream);
private:
	int vcid;  //video codec id, default is CODEC_ID_AVC = 7
	vector<char> segmentplayload;
	FILE* m_segmentfile;
};

class tsmuxer
{
public:
	tsmuxer();
	~tsmuxer();

	tsmuxer(const tsmuxer& muxer);
	tsmuxer& operator=(const tsmuxer& muxer);

	int on_video(const char* data, const int length);
	int write_video(avcsample& sample);
	int cache_video(avcsample& sample, vector<char>& video);
private:
	tspacket* create_pat_packet(int pmtnumber, int pmtpid);
	tspacket* create_pmt_packet(int pmtnumber);
	/*pes packet need any more for one frame so need split more ts packet*/
	tspacket* create_first_pes(int size);
	tspacket* create_continue_pes();
private:
	avccodec* m_avccodec;
	vector<char> m_videoplayload;  //AVC playload
	tssegment* m_segment;
	unsigned char m_continuity_counter : 4;

	
};

#endif