/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/
/*
 * File:   easy_aac_encoder.h
 * Author: Wellsen@easydarwin.org
 *
 * Created on April 11, 2015, 11:44 AM
 */

#pragma once

#include "audio_buffer.h"
#include "decode_to_pcm.h"
#include "pcm_to_aac.h"
#include <memory>
#include <vector>

class G7ToAac
{
  public:
    G7ToAac() = default;
    ~G7ToAac() = default;

    bool init();
    bool init(const InAudioInfo& info);

    int aac_encode(const unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int outcap,
                   unsigned int* outlen);

  private:
    G7ToAac(const G7ToAac&) = delete;
    G7ToAac& operator=(const G7ToAac&) = delete;

    int encode_decoded_input(const unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf,
                             unsigned int outcap, unsigned int* outlen);
    int encode_pcm_input(const unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int outcap,
                         unsigned int* outlen);
    int append_pcm_and_encode(const unsigned char* pcm, unsigned int pcm_len, unsigned char* outbuf,
                              unsigned int outcap, unsigned int* outlen);
    int encode_current_pcm_frame(unsigned char* outbuf, unsigned int outcap, unsigned int* outlen);

    bool CreateDecodePcm();
    bool CreateEncodeAac();
    bool CreateBuffer();

  private:
    unsigned int pcm_buffer_used_ = 0;

    unsigned int pcm_buffer_size_ = 0;
    std::vector<unsigned char> pcm_buffer_;

    unsigned int max_output_bytes_ = 0;
    std::vector<unsigned char> aac_buffer_;

    unsigned int decoded_pcm_size_ = 0;
    std::vector<unsigned char> decoded_pcm_buffer_;

    std::vector<unsigned char> compressed_frame_buffer_;
    unsigned int compressed_frame_size_ = 0;

    audio_buffer m_audio_buffer_;

    InAudioInfo m_inAudioInfo;

    std::unique_ptr<IDecodeToPcm> m_pDecodeToPcm;
    std::unique_ptr<PcmToAac> m_pPCMToAAC;
};
