/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "IDecodeToPcm.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

InAudioInfo::InAudioInfo()
{
    InitParam &initParam = m_initparam;
    initParam.u32AudioSamplerate = 8000;
    initParam.ucAudioChannel = 1;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_ALaw;
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
    int16_t *dst = reinterpret_cast<int16_t *>(pout_buf);
    const uint8_t *src = reinterpret_cast<const uint8_t *>(pin_buf);
    uint32_t i = 0;
    int Ret = 0;

    if ((nullptr == pout_buf) || (nullptr == pout_len) || (nullptr == pin_buf) || (0 == in_len))
    {
        return -1;
    }

    if (*pout_len < 2 * in_len)
    {
        return -2;
    }
    //---{{{

    for (i = 0; i < in_len; i++)
    {
        *(dst++) = (int16_t)DecodeOneChar(*(src++));
    }

    //---}}}
    *pout_len = 2 * in_len;

    Ret = 2 * in_len;
    return Ret;
}
