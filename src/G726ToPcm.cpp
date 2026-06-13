/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "G726ToPcm.h"
#include "condef.h"

G726ToPcm::G726ToPcm(void)
    : m_state726(NULL), m_bitRate(0), m_g7FrameSize(0), m_pcmSize(0)
{
}

G726ToPcm::~G726ToPcm(void)
{
    SAFE_FREE_BUF(m_state726);
}

int G726ToPcm::Init(InAudioInfo info)
{
    m_g7FrameSize = G711_ONE_LEN;
    m_state726 = (g726_state_t*)malloc(sizeof(g726_state_t));
    if (m_state726 == NULL)
    {
        return -1;
    }

    switch (info.G726RateBits())
    {
    case Rate16kBits:
        m_bitRate = 8000 * 2;
        m_pcmSize = (2 * m_g7FrameSize * 120 + 30) / 30;
        break;
    case Rate24kBits:
        m_bitRate = 8000 * 3;
        m_pcmSize = (2 * m_g7FrameSize * 80 + 30) / 30;
        break;
    case Rate32kBits:
        m_bitRate = 8000 * 4;
        m_pcmSize = (2 * m_g7FrameSize * 60 + 30) / 30;
        break;
    case Rate40kBits:
        m_bitRate = 8000 * 5;
        m_pcmSize = (2 * m_g7FrameSize * 48 + 30) / 30;
        break;
    default:
        SAFE_FREE_BUF(m_state726);
        return -1;
    }

    m_state726 = g726_init(m_state726, m_bitRate);
    return (m_state726 == NULL) ? -1 : 0;
}

int G726ToPcm::Decode(unsigned char* outbuf, unsigned int* outlen, unsigned char* inbuf, unsigned int inlen)
{
    if (m_state726 == NULL || outbuf == NULL || outlen == NULL || inbuf == NULL || inlen == 0)
    {
        return -1;
    }
    if (*outlen < static_cast<unsigned int>(m_pcmSize))
    {
        return -2;
    }

    int decoded_samples = g726_decode(m_state726, (short*)outbuf, inbuf, inlen);
    if (decoded_samples < 0)
    {
        return -1;
    }

    *outlen = static_cast<unsigned int>(decoded_samples * 2);
    return decoded_samples * 2;
}

int G726ToPcm::PCMSize()
{
    return m_pcmSize;
}

int G726ToPcm::G7FrameSize()
{
    return m_g7FrameSize;
}
