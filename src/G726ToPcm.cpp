/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "G726ToPcm.h"
#include <cstdlib>

void G726StateDeleter::operator()(g726_state_t* state) const
{
    std::free(state);
}

int G726ToPcm::Init(InAudioInfo info)
{
    m_g7FrameSize = G711_ONE_LEN;
    m_state726.reset(static_cast<g726_state_t*>(std::malloc(sizeof(g726_state_t))));
    if (!m_state726)
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
        m_state726.reset();
        return -1;
    }

    if (g726_init(m_state726.get(), m_bitRate) == nullptr)
    {
        m_state726.reset();
        return -1;
    }
    return 0;
}

int G726ToPcm::Decode(unsigned char* outbuf, unsigned int* outlen, const unsigned char* inbuf, unsigned int inlen)
{
    if (!m_state726 || outbuf == nullptr || outlen == nullptr || inbuf == nullptr || inlen == 0)
    {
        return -1;
    }
    if (*outlen < static_cast<unsigned int>(m_pcmSize))
    {
        return -2;
    }

    int decoded_samples = g726_decode(m_state726.get(), reinterpret_cast<short*>(outbuf), inbuf, inlen);
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
