/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/
/*
 * File:   main.cpp
 * Author: Wellsen@easydarwin.org
 *
 * Created on April 11, 2015, 6:38 PM
 */

#include "../EasyAACEncoderAPI.h"
#include <cstdio>
#include <memory>
#include <vector>

// #define TEST_G711A_FILE       "src.g711a"  // Private
#define TEST_G711A_FILE "g711.g711a" // Standard

#define TEST_AAC_FILE "dest.aac"

int TestG711ToAAC_private();
int TestG711ToAAC_standard();
int TestG726ToAAC();

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
    TestG726ToAAC();
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
    EasyAACEncoder_Handle handle = Easy_AACEncoder_Init(initParam);
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

    EasyAACEncoder_Handle handle = Easy_AACEncoder_Init(initParam);
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

    const unsigned int bG726BufferSize = 500;
    const unsigned int bAACBufferSize = 4 * bG726BufferSize; // Provide a sufficiently large buffer
    std::vector<unsigned char> pbG726Buffer(bG726BufferSize);
    std::vector<unsigned char> pbAACBuffer(bAACBufferSize);
    unsigned int out_len = 0;

    size_t gBytesRead = 0;
    while ((gBytesRead = std::fread(pbG726Buffer.data(), 1, pbG726Buffer.size(), fpIn.get())) > 0)
    {
        if (Easy_AACEncoder_Encode(handle, pbG726Buffer.data(), static_cast<unsigned int>(gBytesRead),
                                   pbAACBuffer.data(), &out_len) > 0)
        {
            std::fwrite(pbAACBuffer.data(), 1, out_len, fpOut.get());
            std::printf("%s:[%d] pbAACBuffer(%u) len=%u \n", __FUNCTION__, __LINE__, bAACBufferSize, out_len);
        }
    }

    Easy_AACEncoder_Release(handle);

    return 0;
}
