/*
    Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "EasyAACEncoderAPI.h"
#include <cstdio>
#include <memory>
#include <vector>

// #define TEST_G711A_FILE       "src.g711a"  // Private
#define TEST_G711A_FILE "g711.g711a" // Standard

#define TEST_AAC_FILE "dest.aac"

int TestG711ToAAC_private();
int TestG711ToAAC_standard();
int TestG726ToAAC();
int TestPcmToAAC();

struct FileCloser
{
    void operator()(FILE* file) const
    {
        if (file != nullptr)
        {
            std::fclose(file);
        }
    }
};

using FilePtr = std::unique_ptr<FILE, FileCloser>;

static FilePtr open_file(const char* path, const char* mode)
{
    return FilePtr(std::fopen(path, mode));
}

int main(int, char**)
{
    // TestG711ToAAC_private();
    // TestG711ToAAC_standard();
    // TestG726ToAAC();
    TestPcmToAAC();
    return 0;
}

int TestG711ToAAC_standard()
{
    InitParam initParam;
    initParam.u32AudioSamplerate = 8000;
    initParam.ucAudioChannel = 1;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_ALaw;
    // initParam.ucAudioCodec = Law_ULaw;

    Easy_Handle handle = Easy_AACEncoder_Init(initParam);
    if (handle == nullptr)
    {
        std::printf("%s:[%d] init encoder failed\n", __FUNCTION__, __LINE__);
        return -1;
    }
    const char* infilename = "g711.g711a"; // Standard
    const char* outAacname = "g711.aac";

    auto fpIn = open_file(infilename, "rb");
    if (!fpIn)
    {
        std::printf("%s:[%d] open %s file failed\n", __FUNCTION__, __LINE__, infilename);
        Easy_AACEncoder_Release(handle);
        return -1;
    }

    auto fpOut = open_file(outAacname, "wb");
    if (!fpOut)
    {
        std::printf("%s:[%d] open %s file failed\n", __FUNCTION__, __LINE__, outAacname);
        Easy_AACEncoder_Release(handle);
        return -1;
    }

    const unsigned int bG711ABufferSize = 500;
    const unsigned int bAACBufferSize = 4 * bG711ABufferSize; // Provide a sufficiently large buffer
    std::vector<unsigned char> pbG711ABuffer(bG711ABufferSize);
    std::vector<unsigned char> pbAACBuffer(bAACBufferSize);
    unsigned int out_len = 0;

    size_t gBytesRead = 0;
    while ((gBytesRead = std::fread(pbG711ABuffer.data(), 1, pbG711ABuffer.size(), fpIn.get())) > 0)
    {
        if (Easy_AACEncoder_Encode(handle, pbG711ABuffer.data(), static_cast<unsigned int>(gBytesRead),
                                   pbAACBuffer.data(), &out_len) > 0)
        {
            std::fwrite(pbAACBuffer.data(), 1, out_len, fpOut.get());
        }
    }

    Easy_AACEncoder_Release(handle);

    return 0;
}

#include <unistd.h>
#include <time.h>
static unsigned long long os_get_reltime()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000000ULL + tp.tv_nsec / 1000ULL);
}

int TestG726ToAAC()
{
    InitParam initParam;
    initParam.u32AudioSamplerate = 8000;
    initParam.ucAudioChannel = 1;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_G726;
    // initParam.g726param.ucRateBits=Rate16kBits;
    // initParam.g726param.ucRateBits=Rate24kBits;
    // initParam.g726param.ucRateBits=Rate32kBits;
    initParam.g726param.ucRateBits = Rate40kBits;

    Easy_Handle handle = Easy_AACEncoder_Init(initParam);
    if (handle == nullptr)
    {
        std::printf("%s:[%d] init encoder failed\n", __FUNCTION__, __LINE__);
        return -1;
    }
    // char* infilename = "encode_out_16.g726";
    // char* outAacname = "encode_out_16.aac";
    // char* infilename = "encode_out_24.g726";
    // char* outAacname = "encode_out_24.aac";
    // char* infilename = "encode_out_32.g726";
    // char* outAacname = "encode_out_32.aac";
    const char* infilename = "encode_out_40.g726";
    const char* outAacname = "encode_out_40.aac";

    auto fpIn = open_file(infilename, "rb");
    if (!fpIn)
    {
        std::printf("%s:[%d] open %s file failed\n", __FUNCTION__, __LINE__, infilename);
        Easy_AACEncoder_Release(handle);
        return -1;
    }

    auto fpOut = open_file(outAacname, "wb");
    if (!fpOut)
    {
        std::printf("%s:[%d] open %s file failed\n", __FUNCTION__, __LINE__, outAacname);
        Easy_AACEncoder_Release(handle);
        return -1;
    }

    const unsigned int bG726BufferSize = 40000 / 8 / 25;
    const unsigned int bAACBufferSize = 4 * bG726BufferSize; // Provide a sufficiently large buffer
    std::vector<unsigned char> pbG726Buffer(bG726BufferSize);
    std::vector<unsigned char> pbAACBuffer(bAACBufferSize);
    unsigned int out_len = 0;
    unsigned long long ts = os_get_reltime();

    size_t gBytesRead = 0;
    while ((gBytesRead = std::fread(pbG726Buffer.data(), 1, pbG726Buffer.size(), fpIn.get())) > 0)
    {
        if (Easy_AACEncoder_Encode(handle, pbG726Buffer.data(), static_cast<unsigned int>(gBytesRead),
                                   pbAACBuffer.data(), &out_len) > 0)
        {
            std::fwrite(pbAACBuffer.data(), 1, out_len, fpOut.get());
            std::printf("%s:[%d] pbAACBuffer(%u) len=%u \n", __FUNCTION__, __LINE__, bAACBufferSize, out_len);
        }
        while (os_get_reltime() < ts + 1000000 / 25)
        {
            usleep(1000000 / 25);
        }
        ts += 1000000 / 25;
    }

    Easy_AACEncoder_Release(handle);

    return 0;
}

int TestPcmToAAC()
{
    InitParam initParam;
    initParam.u32AudioSamplerate = 44100;
    initParam.ucAudioChannel = 2;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_PCM16;
    // initParam.g726param.ucRateBits=Rate16kBits;
    // initParam.g726param.ucRateBits=Rate24kBits;
    // initParam.g726param.ucRateBits=Rate32kBits;
    // initParam.g726param.ucRateBits=Rate40kBits;

    Easy_Handle handle = Easy_AACEncoder_Init(initParam);
    if (handle == nullptr)
    {
        std::printf("%s:[%d] init encoder failed\n", __FUNCTION__, __LINE__);
        return -1;
    }
    // char* infilename = "encode_out_16.g726";
    // char* outAacname = "encode_out_16.aac";
    // char* infilename = "encode_out_24.g726";
    // char* outAacname = "encode_out_24.aac";
    // char* infilename = "encode_out_32.g726";
    // char* outAacname = "encode_out_32.aac";
    const char* infilename = "playback.pcm";
    const char* outAacname = "playback.aac";

    auto fpIn = open_file(infilename, "rb");
    if (!fpIn)
    {
        std::printf("%s:[%d] open %s file failed\n", __FUNCTION__, __LINE__, infilename);
        Easy_AACEncoder_Release(handle);
        return -1;
    }

    auto fpOut = open_file(outAacname, "wb");
    if (!fpOut)
    {
        std::printf("%s:[%d] open %s file failed\n", __FUNCTION__, __LINE__, outAacname);
        Easy_AACEncoder_Release(handle);
        return -1;
    }

    const unsigned int bPcmBufferSize = 44100 * 2 * 16 / 8 / 25;
    const unsigned int bAACBufferSize = 4 * bPcmBufferSize; // Provide a sufficiently large buffer
    std::vector<unsigned char> pbPcmBuffer(bPcmBufferSize);
    std::vector<unsigned char> pbAACBuffer(bAACBufferSize);
    unsigned int out_len = 0;

    size_t gBytesRead = 0;
    while ((gBytesRead = std::fread(pbPcmBuffer.data(), 1, pbPcmBuffer.size(), fpIn.get())) > 0)
    {
        if ((Easy_AACEncoder_Encode(handle, pbPcmBuffer.data(), static_cast<unsigned int>(gBytesRead),
                                    pbAACBuffer.data(), &out_len)) > 0)
        {
            std::fwrite(pbAACBuffer.data(), 1, out_len, fpOut.get());
            std::printf("%s:[%d] pbAACBuffer(%u) len=%u \n", __FUNCTION__, __LINE__, bAACBufferSize, out_len);
        }
    }

    Easy_AACEncoder_Release(handle);

    return 0;
}
