/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once
#include "decode_to_pcm.h"
#include "g726.h"
#include <memory>

class G726ToPcm : public IDecodeToPcm
{
  public:
    G726ToPcm() = default;
    ~G726ToPcm() override = default;
    G726ToPcm(const G726ToPcm&) = delete;
    G726ToPcm& operator=(const G726ToPcm&) = delete;
    G726ToPcm(G726ToPcm&&) = default;
    G726ToPcm& operator=(G726ToPcm&&) = default;

  public:
    int Init(InAudioInfo info) override;

  public:
    int Decode(unsigned char* outbuf, unsigned int* outlen, const unsigned char* inbuf, unsigned int inlen) override;
    int PCMSize() override;
    int G7FrameSize() override;

  private:
    std::unique_ptr<g726_state_t> m_state726;
    int m_bitRate = 0;

    int m_g7FrameSize = 0;
    int m_pcmSize = 0;
};
