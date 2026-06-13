/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "decode_to_pcm.h"

#include <cstdint>
#include <limits>

InAudioInfo::InAudioInfo()
{
    InitParam& initParam = m_initparam;
    initParam.u32AudioSamplerate = 8000;
    initParam.ucAudioChannel = 1;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_ALaw;
    initParam.g711param.reserved = 0;
}
InAudioInfo::InAudioInfo(InitParam param) : m_initparam(param)
{
}
//------------------------------------------------------------------------------------------------------------------------
int DecodeToPcmBase::Init(InAudioInfo info)
{
    (void)info;
    m_g7FrameSize = G711_ONE_LEN;
    return 0;
}
int DecodeToPcmBase::PCMSize()
{
    return CON_PCM_SIZE;
}
int DecodeToPcmBase::G7FrameSize()
{
    return m_g7FrameSize;
}
int DecodeToPcmBase::Decode(unsigned char *pout_buf, unsigned int *pout_len, const unsigned char *pin_buf,
                            unsigned int in_len)
{
    if ((nullptr == pout_buf) || (nullptr == pout_len) || (nullptr == pin_buf) || (0 == in_len))
    {
        return -1;
    }

    if (in_len > std::numeric_limits<unsigned int>::max() / 2)
    {
        return -2;
    }

    const unsigned int output_bytes = 2 * in_len;
    if (*pout_len < output_bytes)
    {
        return -2;
    }

    auto *dst = reinterpret_cast<int16_t *>(pout_buf);
    const auto *src = reinterpret_cast<const uint8_t *>(pin_buf);

    for (uint32_t i = 0; i < in_len; i++)
    {
        *(dst++) = static_cast<int16_t>(DecodeOneChar(*(src++)));
    }

    *pout_len = output_bytes;
    return static_cast<int>(*pout_len);
}
