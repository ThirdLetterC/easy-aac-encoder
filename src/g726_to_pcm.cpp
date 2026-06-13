/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "g726_to_pcm.h"

#include <cstddef>
#include <limits>
#include <utility>

namespace
{
int BitRateFromRateBits(unsigned char rateBits)
{
    switch (rateBits)
    {
    case Rate16kBits:
        return 16000;
    case Rate24kBits:
        return 24000;
    case Rate32kBits:
        return 32000;
    case Rate40kBits:
        return 40000;
    default:
        return 0;
    }
}

int BitsPerSampleFromBitRate(int bitRate)
{
    return bitRate / 8000;
}
} // namespace

int G726ToPcm::Init(InAudioInfo info)
{
    m_state726.reset();
    m_bitRate = BitRateFromRateBits(info.G726RateBits());
    m_g7FrameSize = G711_ONE_LEN;
    m_pcmSize = 0;

    const int bitsPerSample = BitsPerSampleFromBitRate(m_bitRate);
    if (bitsPerSample == 0)
    {
        return -1;
    }

    m_pcmSize = (m_g7FrameSize * 8 / bitsPerSample) * static_cast<int>(sizeof(short));
    auto state = std::make_unique<g726_state_t>();
    if (g726_init(state.get(), m_bitRate) == nullptr)
    {
        return -1;
    }

    m_state726 = std::move(state);
    return 0;
}

int G726ToPcm::Decode(unsigned char* outbuf, unsigned int* outlen, const unsigned char* inbuf, unsigned int inlen)
{
    if (!m_state726 || outbuf == nullptr || outlen == nullptr || inbuf == nullptr || inlen == 0)
    {
        return -1;
    }
    const auto requiredOutputBytes =
        (static_cast<std::size_t>(inlen) * 8 / static_cast<std::size_t>(m_state726->bits_per_sample)) * sizeof(short);
    if (requiredOutputBytes > std::numeric_limits<unsigned int>::max())
    {
        return -2;
    }
    if (*outlen < requiredOutputBytes)
    {
        return -2;
    }

    const int decoded_samples = g726_decode(m_state726.get(), reinterpret_cast<short*>(outbuf), inbuf, inlen);
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
