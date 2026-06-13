/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "decode_to_pcm.h"

extern "C"
{
#include <faac.h>
}

class PcmToAac
{
  public:
    PcmToAac() = default;
    ~PcmToAac();
    PcmToAac(const PcmToAac&) = delete;
    PcmToAac& operator=(const PcmToAac&) = delete;

  public:
    bool Init(const InAudioInfo *info);
    int Encode(const int32_t *inputBuffer, unsigned int samplesInput, unsigned char *outputBuffer,
               unsigned int bufferSize);

  public:
    unsigned int GetPCMBitSize()
    {
        return m_nPCMBitSize;
    }
    unsigned int GetInputSamples()
    {
        return m_nInputSamples;
    }
    unsigned int GetMaxOutputBytes()
    {
        return m_nMaxOutputBytes;
    }
    unsigned int GetPCMBufferSize()
    {
        return (m_nInputSamples * (m_nPCMBitSize / 8));
    }

  private:
    faacEncHandle hEncoder = nullptr;
    faacEncConfigurationPtr pConfiguration = nullptr;

    unsigned int m_nPCMBitSize = 16;

    unsigned long m_nInputSamples = 0;

    unsigned long m_nMaxOutputBytes = 0;
};
