#include "avccodec.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

avcsample::avcsample() :isIDR(false)
{

}

avcsample::~avcsample()
{

}

avcsample::avcsample(const avcsample& sample)
{
	
}

avcsample& avcsample::operator = (const avcsample& sample)
{

}

int avcsample::add_sample(const char* data, const int length)
{
	char* newsample = new char[length];
	memcpy(newsample,data,length);
	m_nalusamples.push_back(newsample);
	if ((data[0] & 0x1f) == SrsAvcNaluTypeIDR)
	{
		isIDR = true;
	}
}


avccodec::avccodec() :sequenceParameterSetNALUnit(NULL),
					pictureParameterSetNALUnit(NULL)
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
	else if (AVCPacketType == 1)  //One or more NALUs (Full frames are required)
	{
		avcsample sample;
		avc_ibmf_format_demux(data + index, length - index, sample);   //Ŀǰ������video������ibmf��ʽ��"ISO Base Media File Format"
	}
}

int avccodec::avc_ibmf_format_demux(const char* data, const int length, avcsample& sample)
{
	assert(m_NalunitLength != 2);
	int nalulen = 0;
	switch (m_NalunitLength)
	{
		case 0:
			nalulen = 1; break;
		case 1:
			nalulen = 2; break;
		case 3:
			nalulen = 4; break;
		default:
		{	
			printf("AVC nalu length size is error,It must be 0,1,3\n");
			return -1;
		}break;
	}

	for (int i = 0; i < length; i++)
	{
		int readnalulen = 0;
		int num = 0;
		char* p = (char*)(&readnalulen);
		if (nalulen == 1)
		{
			*p = data[num];
			num += 1;
		}
		else if (nalulen == 2)
		{
			p[1] = data[num];
			num += 1;
			p[0] = data[num];
			num += 1;
		}
		else if (nalulen == 4)
		{
			p[3] = data[num];
			num += 1;
			p[2] = data[num];
			num += 1;
			p[1] = data[num];
			num += 1;
			p[0] = data[num];
			num += 1;
		}
		else
		{
			printf("nalu len is error\n");
			return -1;
		}
		sample.add_sample(data + nalulen, readnalulen);
		i += (nalulen + readnalulen);
		data += (nalulen + readnalulen);
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
	m_NalunitLength = lengthSizeMinusOne;
	index += 1;
	unsigned char numOfSequenceParameterSets = data[index] & 0x1f;
	index += 1;
	for (int i = 0; i < numOfSequenceParameterSets; i++)   //Ŀǰ��ԵĶ���sps����Ϊһ����
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
	for (int i = 0; i < numOfPictureParameterSets; i++)      //Ŀǰ��ԵĶ���pps����Ϊһ����
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