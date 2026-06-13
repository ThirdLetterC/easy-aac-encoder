/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "PcmToAac.h"

#include "outDebug.h"

PcmToAac::PcmToAac(void)
    : hEncoder(NULL), pConfiguration(NULL)
{
    m_nInputSamples = 0;
    m_nMaxOutputBytes = 0;
    m_nPCMBitSize = 16;
}

PcmToAac::~PcmToAac(void)
{
    if (NULL != hEncoder)
    {
        /*Close FAAC engine*/
        faacEncClose(hEncoder);
    }
}

bool PcmToAac::Init(InAudioInfo* info)
{
    if (info == NULL || info->Channel() == 0 || info->Samplerate() == 0 || info->PCMBitSize() != 16)
    {
        return false;
    }

    unsigned int objectType = LOW;
    unsigned int mpegVersion = MPEG2;
    static unsigned int useTns = 0; // #define DEFAULT_TNS     0

    // TODO: config this
    unsigned int nChannels = /*1*/ info->Channel();

    m_nPCMBitSize = /*16*/ info->PCMBitSize();
    unsigned long nInputSamples = 0;
    unsigned long nSampleRate = /*8000*/ info->Samplerate();
    unsigned long nMaxOutputBytes = 0;

    /*open FAAC engine*/
    hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);
    if (hEncoder == NULL)
    {
        if (AAC_DEBUG)
            printf("%s:[%d] failed to call faacEncOpen !\n", __FUNCTION__, __LINE__);
        // return -1;
        return false;
    }
    m_nInputSamples = nInputSamples;
    m_nMaxOutputBytes = nMaxOutputBytes;

    /*get current encoding configuration*/
    pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
    if (pConfiguration == NULL)
    {
        faacEncClose(hEncoder);
        hEncoder = NULL;
        return false;
    }
    pConfiguration->inputFormat = FAAC_INPUT_16BIT;

    /*0 - raw; 1 - ADTS*/
    pConfiguration->outputFormat = 1;
    pConfiguration->useTns = useTns;
    pConfiguration->aacObjectType = objectType;
    pConfiguration->mpegVersion = mpegVersion;

    /*set encoding configuretion*/
    if (!faacEncSetConfiguration(hEncoder, pConfiguration))
    {
        faacEncClose(hEncoder);
        hEncoder = NULL;
        return false;
    }

    return true;
}

int PcmToAac::Encode(const int32_t* pbPCMBuffer, unsigned int nPCMBufferSize, unsigned char* pbAACBuffer,
                     unsigned int nMaxOutputBytes)
{
    if (hEncoder == NULL || pbPCMBuffer == NULL || pbAACBuffer == NULL || nMaxOutputBytes == 0)
    {
        return EasyAACEncoder_InvalidArgument;
    }

    unsigned int nPCMBitSize = GetPCMBitSize();

    unsigned int nInputSamples = (nPCMBufferSize / (nPCMBitSize / 8));
    if (AAC_DEBUG)
        printf("%s:[%d] G711A -> PCM faacEncEncode....\n", __FUNCTION__, __LINE__);
    int nRet = faacEncEncode(hEncoder, (int32_t*)pbPCMBuffer, nInputSamples, pbAACBuffer, nMaxOutputBytes);
    if (AAC_DEBUG)
        printf("%s:[%d] G711A -> PCM faacEncEncode\n", __FUNCTION__, __LINE__);

    return nRet;
}
