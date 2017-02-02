#include "avccodec.h"
#include <stdio.h>
#include <string.h>
avccodec::avccodec() :sequenceParameterSetNALUnit(NULL)
{

}

avccodec::~avccodec()
{

}

avccodec::avccodec(const avccodec& codec)
{

}

avccodec& avccodec::operator = (const avccodec& codec)
{

}

int avccodec::avcdemux(const char* data, const int length)
{
	int index = 0;
	printf("start demux avc data\n");
	char frametype = (data[index] >> 4) & 0x0f;
	char codecid = (data[index] & 0x0f);
	if (frametype == FRAME_TYPE_inf_command_FRAME || codecid != CODEC_ID_AVC)
	{
		printf("ignore the info and command frame\n");
		return -1;
	}
	if (codecid != CODEC_ID_AVC)
	{
		return -1;
	}

	index += 1;
	char AVCPacketType = data[index];
	index += 1;
	int CompositionTime = 0x00;;
	((char*)(&CompositionTime))[2] = data[index];
	index += 1;
	((char*)(&CompositionTime))[1] = data[index];
	index += 1;
	((char*)(&CompositionTime))[0] = data[index];
	index += 1;
	if (AVCPacketType == 0)
	{
		//This is sequence head and demux sps and pps
		avc_decoder_configuration_record_demux(data + index, length - index);
	}
	else
	{

	}
}

int avccodec::avc_decoder_configuration_record_demux(const char* data, const int length)
{
	printf("decode sps and pps \n");
	int index = 0;
	unsigned char configurationVersion = data[index];
	if (configurationVersion != 1)
	{
		printf("not configurationVersion value is 1\n");
		return -1;
	}
	index += 1;
	unsigned char AVCProfileIndication = data[index];
	index += 1;
	unsigned char profile_compatibility = data[index];
	index += 1;
	unsigned char AVCLevelIndication = data[index];
	index += 1;
	unsigned char lengthSizeMinusOne = data[index] & 0x03;
	index += 1;
	unsigned char numOfSequenceParameterSets = data[index] & 0x1f;
	index += 1;
	for (int i = 0; i < numOfSequenceParameterSets; i++)   //目前针对的都是sps个数为一个的
	{
		int sequenceParameterSetLength = 0;
		((char*)(&sequenceParameterSetLength))[1] = data[index];
		index += 1;
		((char*)(&sequenceParameterSetLength))[0] = data[index];
		index += 1;
		sequenceParameterSetNALUnit = new char[sequenceParameterSetLength];
		memset(sequenceParameterSetNALUnit, 0, sequenceParameterSetLength);
		memcpy(sequenceParameterSetNALUnit, data + index, sequenceParameterSetLength);
		index += sequenceParameterSetLength;
	}
	unsigned char numOfPictureParameterSets = data[index];
	index += 1;
	for (int i = 0; i < numOfPictureParameterSets; i++)      //目前针对的都是pps个数为一个的
	{
		int pictureParameterSetLength = 0;
		((char*)(&pictureParameterSetLength))[1] = data[index];
		index += 1;
		((char*)(&pictureParameterSetLength))[0] = data[index];
		index += 1;
		pictureParameterSetNALUnit = new char[pictureParameterSetLength];
		memcpy(pictureParameterSetNALUnit, data + index, pictureParameterSetLength);
		index += pictureParameterSetLength;
	}
	return 0;
 }