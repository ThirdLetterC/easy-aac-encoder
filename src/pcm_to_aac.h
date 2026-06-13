/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once

#include "decode_to_pcm.h"

#include <cstdint>
#include <memory>

extern "C"
{
#include <faac.h>
}

struct FaacEncoderDeleter
{
    void operator()(faacEncHandle encoder) const noexcept;
};

class PcmToAac
{
  public:
    PcmToAac() = default;
    ~PcmToAac() = default;
    PcmToAac(const PcmToAac&) = delete;
    PcmToAac& operator=(const PcmToAac&) = delete;
    PcmToAac(PcmToAac&&) = default;
    PcmToAac& operator=(PcmToAac&&) = default;

  public:
    bool Init(const InAudioInfo *info);
    int Encode(const int32_t *inputBuffer, unsigned int samplesInput, unsigned char *outputBuffer,
               unsigned int bufferSize);

  public:
    unsigned int GetPCMBitSize() const
    {
        return m_nPCMBitSize;
    }
    unsigned int GetInputSamples() const
    {
        return m_nInputSamples;
    }
    unsigned int GetMaxOutputBytes() const
    {
        return m_nMaxOutputBytes;
    }
    unsigned int GetPCMBufferSize() const
    {
        return (m_nInputSamples * (m_nPCMBitSize / 8));
    }

  private:
    std::unique_ptr<void, FaacEncoderDeleter> hEncoder;
    faacEncConfigurationPtr pConfiguration = nullptr;

    unsigned int m_nPCMBitSize = 16;

    unsigned long m_nInputSamples = 0;

    unsigned long m_nMaxOutputBytes = 0;
};
