/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once
#include "IDecodeToPcm.h"
#include "g726.h"
#include <memory>

struct G726StateDeleter
{
    void operator()(g726_state_t* state) const;
};

class G726ToPcm : public IDecodeToPcm
{
  public:
    G726ToPcm() = default;
    ~G726ToPcm() override = default;
    G726ToPcm(const G726ToPcm&) = delete;
    G726ToPcm& operator=(const G726ToPcm&) = delete;

  public:
    int Init(InAudioInfo info) override;

  public:
    int Decode(unsigned char* outbuf, unsigned int* outlen, const unsigned char* inbuf, unsigned int inlen) override;
    int PCMSize() override;
    int G7FrameSize() override;

  private:
    std::unique_ptr<g726_state_t, G726StateDeleter> m_state726;
    int m_bitRate = 0;

    int m_g7FrameSize = 0;
    int m_pcmSize = 0;
};
