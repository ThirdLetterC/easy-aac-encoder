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

namespace
{
struct FileCloser
{
    void operator()(FILE* file) const noexcept
    {
        if (file != nullptr)
        {
            std::fclose(file);
        }
    }
};

struct EncoderCloser
{
    void operator()(void* handle) const noexcept
    {
        Easy_AACEncoder_Release(handle);
    }
};

using FilePtr = std::unique_ptr<FILE, FileCloser>;
using EncoderPtr = std::unique_ptr<void, EncoderCloser>;

FilePtr open_file(const char* path, const char* mode)
{
    return FilePtr(std::fopen(path, mode));
}

int encode_file(const char* input_path, const char* output_path, const InitParam& init, unsigned int input_chunk_size,
                unsigned int output_capacity)
{
    EncoderPtr handle(Easy_AACEncoder_Init(init));
    if (!handle)
    {
        std::fprintf(stderr, "failed to initialize encoder\n");
        return -1;
    }

    auto input = open_file(input_path, "rb");
    if (!input)
    {
        std::fprintf(stderr, "failed to open input file: %s\n", input_path);
        return -1;
    }

    auto output = open_file(output_path, "wb");
    if (!output)
    {
        std::fprintf(stderr, "failed to open output file: %s\n", output_path);
        return -1;
    }

    std::vector<unsigned char> input_buffer(input_chunk_size);
    std::vector<unsigned char> output_buffer(output_capacity);

    while (true)
    {
        const std::size_t bytes_read = std::fread(input_buffer.data(), 1, input_buffer.size(), input.get());
        if (bytes_read == 0)
        {
            break;
        }

        unsigned int bytes_written = 0;
        const int encode_result =
            Easy_AACEncoder_EncodeEx(handle.get(), input_buffer.data(), static_cast<unsigned int>(bytes_read),
                                     output_buffer.data(), static_cast<unsigned int>(output_buffer.size()),
                                     &bytes_written);
        if (encode_result < 0)
        {
            std::fprintf(stderr, "encode failed: %d\n", encode_result);
            return encode_result;
        }

        if (bytes_written > 0)
        {
            std::fwrite(output_buffer.data(), 1, bytes_written, output.get());
        }
    }

    return 0;
}
} // namespace

int TestG711ToAAC_standard()
{
    InitParam initParam{};
    initParam.u32AudioSamplerate = 8000;
    initParam.ucAudioChannel = 1;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_ALaw;

    constexpr unsigned int inputChunkSize = 500;
    constexpr unsigned int outputCapacity = 4 * inputChunkSize;
    return encode_file("g711.g711a", "g711.aac", initParam, inputChunkSize, outputCapacity);
}

int TestG726ToAAC()
{
    InitParam initParam{};
    initParam.u32AudioSamplerate = 8000;
    initParam.ucAudioChannel = 1;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_G726;
    initParam.g726param.ucRateBits = Rate40kBits;

    constexpr unsigned int inputChunkSize = 40000 / 8 / 25;
    constexpr unsigned int outputCapacity = 4 * inputChunkSize;
    return encode_file("encode_out_40.g726", "encode_out_40.aac", initParam, inputChunkSize, outputCapacity);
}

int TestPcmToAAC()
{
    InitParam initParam{};
    initParam.u32AudioSamplerate = 44100;
    initParam.ucAudioChannel = 2;
    initParam.u32PCMBitSize = 16;
    initParam.ucAudioCodec = Law_PCM16;

    constexpr unsigned int inputChunkSize = 44100 * 2 * 16 / 8 / 25;
    constexpr unsigned int outputCapacity = 4 * inputChunkSize;
    return encode_file("playback.pcm", "playback.aac", initParam, inputChunkSize, outputCapacity);
}

int main()
{
    return TestPcmToAAC();
}
