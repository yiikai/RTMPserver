#ifndef AVC_CODEC_H
#define AVC_CODEC_H

#include <vector>
using namespace std;
/*
1 = key frame(for AVC, a seekable frame)
2 = inter frame(for AVC, a non - seekable frame)
3 = disposable inter frame(H.263 only)
4 = generated key frame(reserved for server use only)
5 = video info / command frame
*/
#define FRAME_TYPE_KEY_FRAME 1
#define FRAME_TYPE_inter_FRAME 2
#define FRAME_TYPE_disposable_inter_FRAME 3
#define FRAME_TYPE_generated_key_FRAME 4
#define FRAME_TYPE_inf_command_FRAME 5

enum SrsAvcNaluType
{
	// Unspecified
	SrsAvcNaluTypeReserved = 0,

	// Coded slice of a non-IDR picture slice_layer_without_partitioning_rbsp( )
	SrsAvcNaluTypeNonIDR = 1,
	// Coded slice data partition A slice_data_partition_a_layer_rbsp( )
	SrsAvcNaluTypeDataPartitionA = 2,
	// Coded slice data partition B slice_data_partition_b_layer_rbsp( )
	SrsAvcNaluTypeDataPartitionB = 3,
	// Coded slice data partition C slice_data_partition_c_layer_rbsp( )
	SrsAvcNaluTypeDataPartitionC = 4,
	// Coded slice of an IDR picture slice_layer_without_partitioning_rbsp( )
	SrsAvcNaluTypeIDR = 5,
	// Supplemental enhancement information (SEI) sei_rbsp( )
	SrsAvcNaluTypeSEI = 6,
	// Sequence parameter set seq_parameter_set_rbsp( )
	SrsAvcNaluTypeSPS = 7,
	// Picture parameter set pic_parameter_set_rbsp( )
	SrsAvcNaluTypePPS = 8,
	// Access unit delimiter access_unit_delimiter_rbsp( )
	SrsAvcNaluTypeAccessUnitDelimiter = 9,
	// End of sequence end_of_seq_rbsp( )
	SrsAvcNaluTypeEOSequence = 10,
	// End of stream end_of_stream_rbsp( )
	SrsAvcNaluTypeEOStream = 11,
	// Filler data filler_data_rbsp( )
	SrsAvcNaluTypeFilterData = 12,
	// Sequence parameter set extension seq_parameter_set_extension_rbsp( )
	SrsAvcNaluTypeSPSExt = 13,
	// Prefix NAL unit prefix_nal_unit_rbsp( )
	SrsAvcNaluTypePrefixNALU = 14,
	// Subset sequence parameter set subset_seq_parameter_set_rbsp( )
	SrsAvcNaluTypeSubsetSPS = 15,
	// Coded slice of an auxiliary coded picture without partitioning slice_layer_without_partitioning_rbsp( )
	SrsAvcNaluTypeLayerWithoutPartition = 19,
	// Coded slice extension slice_layer_extension_rbsp( )
	SrsAvcNaluTypeCodedSliceExt = 20,
};


/*
2 = Sorenson H.263
3 = Screen video
4 = On2 VP6
5 = On2 VP6 with alpha channel
6 = Screen video version 2
7 = AVC
*/

#define CODEC_ID_AVC 7

class avcsample
{
public:
	avcsample();
	~avcsample();

	avcsample(const avcsample& sample);
	avcsample& operator=(const avcsample& sample);

	int add_sample(const char* data, const int length);
private:
	vector<char*> m_nalusamples;     //AVC spec 每一帧都是由很多个nalu组合起来的
	bool isIDR;
};



class avccodec
{
public:
	avccodec();
	~avccodec();

	avccodec(const avccodec& codec);
	avccodec& operator=(const avccodec& codec);

	int avcdemux(const char* data, const int length);
private:
	int avc_decoder_configuration_record_demux(const char* data, const int length);
	int avc_ibmf_format_demux(const char* data, const int length, avcsample& sample);
private:
	char* sequenceParameterSetNALUnit;
	char* pictureParameterSetNALUnit;

	unsigned int m_NalunitLength;
};

#endif