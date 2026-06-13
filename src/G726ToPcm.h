/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once
#include "IDecodeToPcm.h"
#include "g726.h"

class G726ToPcm : public IDecodeToPcm
{
  public:
    G726ToPcm(void);
    virtual ~G726ToPcm(void);
    G726ToPcm(const G726ToPcm&) = delete;
    G726ToPcm& operator=(const G726ToPcm&) = delete;

  public:
    int Init(InAudioInfo info) override;

  public:
    int Decode(unsigned char* outbuf, unsigned int* outlen, const unsigned char* inbuf, unsigned int inlen) override;
    int PCMSize() override;
    int G7FrameSize() override;

  private:
    g726_state_t* m_state726;
    int m_bitRate;

    int m_g7FrameSize;
    int m_pcmSize;
};
