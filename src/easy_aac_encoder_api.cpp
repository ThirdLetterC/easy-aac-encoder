/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/
/*
 * File:   libEasyAACEncoder.cpp
 * Author: Wellsen@easydarwin.org
 *
 * Created on April 11, 2015, 10:57 AM
 */

#include "EasyAACEncoderAPI.h"
#include "easy_aac_encoder.h"
#include <limits>
#include <memory>

Easy_API Easy_Handle Easy_APICALL Easy_AACEncoder_Init(InitParam initPar)
{
    auto encoder = std::make_unique<G7ToAac>();
    InAudioInfo info(initPar);
    if (!encoder->init(info))
    {
        return nullptr;
    }
    return encoder.release();
}

Easy_API int Easy_APICALL Easy_AACEncoder_Encode(Easy_Handle handle, const unsigned char* inbuf, unsigned int inlen,
                                                 unsigned char* outbuf, unsigned int* outlen)
{
    return Easy_AACEncoder_EncodeEx(handle, inbuf, inlen, outbuf, std::numeric_limits<unsigned int>::max(), outlen);
}

Easy_API int Easy_APICALL Easy_AACEncoder_EncodeEx(Easy_Handle handle, const unsigned char* inbuf, unsigned int inlen,
                                                   unsigned char* outbuf, unsigned int outcap, unsigned int* outlen)
{
    if (handle == nullptr)
    {
        return EasyAACEncoder_InvalidArgument;
    }

    return static_cast<G7ToAac*>(handle)->aac_encode(inbuf, inlen, outbuf, outcap, outlen);
}

Easy_API void Easy_APICALL Easy_AACEncoder_Release(Easy_Handle handle)
{
    if (handle != nullptr)
    {
        delete static_cast<G7ToAac*>(handle);
    }
}
