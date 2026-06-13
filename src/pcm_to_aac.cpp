/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "pcm_to_aac.h"

#include "out_debug.h"

#include <cstdio>

void FaacEncoderDeleter::operator()(faacEncHandle encoder) const noexcept
{
    if (encoder != nullptr)
    {
        faacEncClose(encoder);
    }
}

bool PcmToAac::Init(const InAudioInfo* info)
{
    hEncoder.reset();
    pConfiguration = nullptr;
    m_nInputSamples = 0;
    m_nMaxOutputBytes = 0;

    if (info == nullptr || info->Channel() == 0 || info->Samplerate() == 0 || info->PCMBitSize() != 16)
    {
        return false;
    }

    constexpr unsigned int objectType = LOW;
    constexpr unsigned int mpegVersion = MPEG2;
    constexpr unsigned int useTns = 0;

    const unsigned int nChannels = info->Channel();

    m_nPCMBitSize = info->PCMBitSize();
    unsigned long nInputSamples = 0;
    const unsigned long nSampleRate = info->Samplerate();
    unsigned long nMaxOutputBytes = 0;

    faacEncHandle rawEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);
    if (rawEncoder == nullptr)
    {
        if (AAC_DEBUG)
        {
            std::printf("%s:[%d] failed to call faacEncOpen !\n", __FUNCTION__, __LINE__);
        }
        return false;
    }
    hEncoder.reset(rawEncoder);
    m_nInputSamples = nInputSamples;
    m_nMaxOutputBytes = nMaxOutputBytes;

    pConfiguration = faacEncGetCurrentConfiguration(hEncoder.get());
    if (pConfiguration == nullptr)
    {
        hEncoder.reset();
        return false;
    }
    pConfiguration->inputFormat = FAAC_INPUT_16BIT;

    /*0 - raw; 1 - ADTS*/
    pConfiguration->outputFormat = 1;
    pConfiguration->useTns = useTns;
    pConfiguration->aacObjectType = objectType;
    pConfiguration->mpegVersion = mpegVersion;

    if (!faacEncSetConfiguration(hEncoder.get(), pConfiguration))
    {
        hEncoder.reset();
        return false;
    }

    return true;
}

int PcmToAac::Encode(const int32_t* pbPCMBuffer, unsigned int nPCMBufferSize, unsigned char* pbAACBuffer,
                     unsigned int nMaxOutputBytes)
{
    if (!hEncoder || pbPCMBuffer == nullptr || pbAACBuffer == nullptr || nMaxOutputBytes == 0)
    {
        return EasyAACEncoder_InvalidArgument;
    }

    const unsigned int nPCMBitSize = GetPCMBitSize();
    if (nPCMBitSize == 0)
    {
        return EasyAACEncoder_InvalidArgument;
    }

    const unsigned int nInputSamples = nPCMBufferSize / (nPCMBitSize / 8);
    if (AAC_DEBUG)
    {
        std::printf("%s:[%d] G711A -> PCM faacEncEncode....\n", __FUNCTION__, __LINE__);
    }
    int nRet = faacEncEncode(hEncoder.get(), const_cast<int32_t*>(pbPCMBuffer), nInputSamples, pbAACBuffer,
                             nMaxOutputBytes);
    if (AAC_DEBUG)
    {
        std::printf("%s:[%d] G711A -> PCM faacEncEncode\n", __FUNCTION__, __LINE__);
    }

    return nRet;
}
