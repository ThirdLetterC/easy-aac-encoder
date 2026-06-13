/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once

#ifndef _IDECODETOPCM_H
#define _IDECODETOPCM_H

#include "EasyAACEncoderAPI.h"

#define USE_SHOUT_G711PACK 1

#if USE_SHOUT_G711PACK
#define G711_ONE_LEN 160
#define G711_ONE_OFFSET 0

#else
#define G711_ONE_LEN 164
#define G711_ONE_OFFSET 4
#endif

inline constexpr int CON_PCM_SIZE = 320;

class audio_buffer;

class InAudioInfo
{
  public:
    InAudioInfo();
    explicit InAudioInfo(InitParam param);
    ~InAudioInfo() = default;

    unsigned int CodecType() const
    {
        return m_initparam.ucAudioCodec;
    }
    unsigned int Samplerate() const
    {
        return m_initparam.u32AudioSamplerate;
    }
    unsigned int Channel() const
    {
        return m_initparam.ucAudioChannel;
    }
    unsigned int PCMBitSize() const
    {
        return m_initparam.u32PCMBitSize;
    }
    unsigned char G726RateBits() const
    {
        return m_initparam.g726param.ucRateBits;
    }

  private:
    InitParam m_initparam;
};
//----------------------------------------------------------
class IDecodeToPcm
{
  public:
    IDecodeToPcm() = default;
    virtual ~IDecodeToPcm() = default;

  public:
    virtual int Init(InAudioInfo info) = 0;
    virtual int Decode(unsigned char* outbuf, unsigned int* outlen, const unsigned char* inbuf, unsigned int inlen) = 0;
    virtual int PCMSize() = 0;
    virtual int G7FrameSize() = 0;
};
//----------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------
class DecodeToPcmBase : public IDecodeToPcm
{
  public:
    DecodeToPcmBase() = default;
    ~DecodeToPcmBase() override = default;

    int Init(InAudioInfo info) override;

  public:
    int Decode(unsigned char* outbuf, unsigned int* outlen, const unsigned char* inbuf, unsigned int inlen) override;
    int PCMSize() override;
    int G7FrameSize() override;

    virtual unsigned short DecodeOneChar(unsigned char data) = 0;

  private:
    int m_g7FrameSize = 0;
};

#endif
