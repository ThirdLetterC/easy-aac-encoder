/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "pcm_to_aac.h"

#include "out_debug.h"

PcmToAac::~PcmToAac()
{
    if (hEncoder != nullptr)
    {
        /*Close FAAC engine*/
        faacEncClose(hEncoder);
    }
}

bool PcmToAac::Init(const InAudioInfo* info)
{
    if (info == nullptr || info->Channel() == 0 || info->Samplerate() == 0 || info->PCMBitSize() != 16)
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
    if (hEncoder == nullptr)
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
    if (pConfiguration == nullptr)
    {
        faacEncClose(hEncoder);
        hEncoder = nullptr;
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
        hEncoder = nullptr;
        return false;
    }

    return true;
}

int PcmToAac::Encode(const int32_t* pbPCMBuffer, unsigned int nPCMBufferSize, unsigned char* pbAACBuffer,
                     unsigned int nMaxOutputBytes)
{
    if (hEncoder == nullptr || pbPCMBuffer == nullptr || pbAACBuffer == nullptr || nMaxOutputBytes == 0)
    {
        return EasyAACEncoder_InvalidArgument;
    }

    unsigned int nPCMBitSize = GetPCMBitSize();

    unsigned int nInputSamples = (nPCMBufferSize / (nPCMBitSize / 8));
    if (AAC_DEBUG)
        printf("%s:[%d] G711A -> PCM faacEncEncode....\n", __FUNCTION__, __LINE__);
    int nRet = faacEncEncode(hEncoder, const_cast<int32_t*>(pbPCMBuffer), nInputSamples, pbAACBuffer, nMaxOutputBytes);
    if (AAC_DEBUG)
        printf("%s:[%d] G711A -> PCM faacEncEncode\n", __FUNCTION__, __LINE__);

    return nRet;
}
