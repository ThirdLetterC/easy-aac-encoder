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

#ifndef EasyAACEncoder_H
#define EasyAACEncoder_H

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
    unsigned int nCount = 0;

    unsigned int m_nPCMBufferSize = 0;
    std::vector<unsigned char> m_pbPCMBuffer;

    unsigned int m_nMaxOutputBytes = 0;
    std::vector<unsigned char> m_pbAACBuffer;

    unsigned int m_nPCMSize = 0;
    std::vector<unsigned char> m_pbPCMTmpBuffer;

    std::vector<unsigned char> m_pbG7FrameBuffer;
    unsigned int m_nG7FrameBufferSize = 0;

    audio_buffer m_audio_buffer_;
    //------
    InAudioInfo m_inAudioInfo;

    std::unique_ptr<IDecodeToPcm> m_pDecodeToPcm;
    std::unique_ptr<PcmToAac> m_pPCMToAAC;
};

#endif /* EasyAACEncoder_H */
