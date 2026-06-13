/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/
/*
 * File:   EasyAACEncoder.cpp
 * Author: Wellsen@easydarwin.org
 *
 * Created on April 11, 2015, 11:44 AM
 */

#include "easy_aac_encoder.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <ranges>

// #include "g711.h"

#include "EasyAACEncoderAPI.h"

#include "out_debug.h"
#include "g711_to_pcm.h"
#include "g726_to_pcm.h"

bool G7ToAac::init()
{
    pcm_buffer_used_ = 0;

    return CreateBuffer();
}

bool G7ToAac::init(const InAudioInfo& info)
{
    m_inAudioInfo = info;

    if (!CreateDecodePcm() || !CreateEncodeAac())
    {
        return false;
    }
    return init();
}
bool G7ToAac::CreateDecodePcm()
{
    if (Law_ALaw == m_inAudioInfo.CodecType())
    {
        m_pDecodeToPcm = std::make_unique<G711AToPcm>();
    }
    else if (Law_ULaw == m_inAudioInfo.CodecType())
    {
        m_pDecodeToPcm = std::make_unique<G711UToPcm>();
    }
    else if (Law_G726 == m_inAudioInfo.CodecType())
    {
        m_pDecodeToPcm = std::make_unique<G726ToPcm>();
    }
    else if (Law_PCM16 == m_inAudioInfo.CodecType())
    {
        m_pDecodeToPcm.reset();
    }
    else
    {
        return false;
    }

    if (m_pDecodeToPcm && m_pDecodeToPcm->Init(m_inAudioInfo) != 0)
    {
        m_pDecodeToPcm.reset();
        return false;
    }

    return true;
}
bool G7ToAac::CreateEncodeAac()
{
    m_pPCMToAAC = std::make_unique<PcmToAac>();
    return m_pPCMToAAC->Init(&m_inAudioInfo);
}
bool G7ToAac::CreateBuffer()
{
    if (!m_pPCMToAAC)
    {
        return false;
    }

    pcm_buffer_size_ = m_pPCMToAAC->GetPCMBufferSize();
    if (pcm_buffer_size_ == 0)
    {
        return false;
    }
    pcm_buffer_.assign(pcm_buffer_size_, 0);

    max_output_bytes_ = m_pPCMToAAC->GetMaxOutputBytes();
    if (max_output_bytes_ == 0)
    {
        return false;
    }
    aac_buffer_.assign(max_output_bytes_, 0);

    if (m_pDecodeToPcm)
    {
        compressed_frame_size_ = m_pDecodeToPcm->G7FrameSize();
        decoded_pcm_size_ = m_pDecodeToPcm->PCMSize();
        if (compressed_frame_size_ == 0 || decoded_pcm_size_ == 0)
        {
            return false;
        }
        compressed_frame_buffer_.assign(compressed_frame_size_, 0);
        decoded_pcm_buffer_.assign(decoded_pcm_size_, 0);
    }
    else
    {
        compressed_frame_size_ = 0;
        decoded_pcm_size_ = 0;
        compressed_frame_buffer_.clear();
        decoded_pcm_buffer_.clear();
    }

    return true;
}
int G7ToAac::aac_encode(const unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int outcap,
                        unsigned int* outlen)
{
    if (outlen == nullptr)
    {
        return EasyAACEncoder_InvalidArgument;
    }
    *outlen = 0;

    if (inlen == 0)
    {
        return 0;
    }

    if (inbuf == nullptr || outbuf == nullptr || outcap == 0 || !m_pPCMToAAC)
    {
        return EasyAACEncoder_InvalidArgument;
    }

    if (m_inAudioInfo.CodecType() == Law_PCM16)
    {
        return encode_pcm_input(inbuf, inlen, outbuf, outcap, outlen);
    }

    if (!m_pDecodeToPcm)
    {
        return EasyAACEncoder_InitFailed;
    }

    return encode_decoded_input(inbuf, inlen, outbuf, outcap, outlen);
}

int G7ToAac::encode_decoded_input(const unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf,
                                  unsigned int outcap, unsigned int* outlen)
{
    if (m_audio_buffer_.write_data(inbuf, static_cast<int>(inlen)) < 0)
    {
        return EasyAACEncoder_BufferError;
    }

    while (true)
    {
        int buffer_len =
            m_audio_buffer_.get_data(compressed_frame_buffer_.data(), static_cast<int>(compressed_frame_size_));
        if (buffer_len == 0)
        {
            break;
        }
        if (buffer_len < 0)
        {
            return EasyAACEncoder_BufferError;
        }

        std::ranges::fill(decoded_pcm_buffer_, 0);
        unsigned int decoded_pcm_size = static_cast<unsigned int>(decoded_pcm_buffer_.size());
        int decoded_pcm_bytes =
            m_pDecodeToPcm->Decode(decoded_pcm_buffer_.data(), &decoded_pcm_size, compressed_frame_buffer_.data(),
                                   static_cast<unsigned int>(buffer_len));
        if (decoded_pcm_bytes < 0)
        {
            return EasyAACEncoder_DecodeFailed;
        }

        int ret = append_pcm_and_encode(decoded_pcm_buffer_.data(), static_cast<unsigned int>(decoded_pcm_bytes),
                                        outbuf, outcap, outlen);
        if (ret < 0)
        {
            return ret;
        }
    }

    return *outlen;
}

int G7ToAac::encode_pcm_input(const unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf,
                              unsigned int outcap, unsigned int* outlen)
{
    return append_pcm_and_encode(inbuf, inlen, outbuf, outcap, outlen);
}

int G7ToAac::append_pcm_and_encode(const unsigned char* pcm, unsigned int pcm_len, unsigned char* outbuf,
                                   unsigned int outcap, unsigned int* outlen)
{
    unsigned int offset = 0;
    while (offset < pcm_len)
    {
        unsigned int space = static_cast<unsigned int>(pcm_buffer_.size()) - pcm_buffer_used_;
        unsigned int to_copy = std::min(space, pcm_len - offset);
        std::copy_n(pcm + offset, to_copy, pcm_buffer_.data() + pcm_buffer_used_);
        pcm_buffer_used_ += to_copy;
        offset += to_copy;

        if (pcm_buffer_used_ == pcm_buffer_.size())
        {
            int ret = encode_current_pcm_frame(outbuf, outcap, outlen);
            if (ret < 0)
            {
                return ret;
            }
            std::ranges::fill(pcm_buffer_, 0);
            pcm_buffer_used_ = 0;
        }
    }

    return *outlen;
}

int G7ToAac::encode_current_pcm_frame(unsigned char* outbuf, unsigned int outcap, unsigned int* outlen)
{
    std::ranges::fill(aac_buffer_, 0);
    std::vector<int32_t> aligned_pcm((pcm_buffer_.size() + sizeof(int32_t) - 1) / sizeof(int32_t), 0);
    std::memcpy(aligned_pcm.data(), pcm_buffer_.data(), pcm_buffer_.size());
    int encoded = m_pPCMToAAC->Encode(aligned_pcm.data(), static_cast<unsigned int>(pcm_buffer_.size()),
                                      aac_buffer_.data(), static_cast<unsigned int>(aac_buffer_.size()));
    if (encoded < 0)
    {
        return EasyAACEncoder_EncodeFailed;
    }
    if (encoded == 0)
    {
        return *outlen;
    }
    if (*outlen > outcap || static_cast<unsigned int>(encoded) > outcap - *outlen)
    {
        return EasyAACEncoder_BufferTooSmall;
    }

    std::copy_n(aac_buffer_.data(), static_cast<unsigned int>(encoded), outbuf + *outlen);
    *outlen += static_cast<unsigned int>(encoded);
    return *outlen;
}
