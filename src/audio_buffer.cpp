#include "audio_buffer.h"
#include "EasyDSSBuffers.h"
#include "outDebug.h"
#include <algorithm>

audio_buffer::audio_buffer()
    : data_(iBufLen), len_(0)
{
}

audio_buffer::~audio_buffer()
{
}

int audio_buffer::write_data(const void* data, int len)
{
    if (data == NULL || len < 0)
    {
        return -1;
    }
    if (static_cast<int>(data_.size()) - len_ < len)
    {
        if (AAC_DEBUG)
            printf("audio_buffer full\n");
        return -1;
    }
    if (len > 0)
    {
        memcpy(data_.data() + len_, data, len);
        len_ += len;
        return len;
    }
    return 0;
}

int audio_buffer::get_data(unsigned char* dest, int how_you_want)
{
    if (dest == NULL || how_you_want <= 0)
    {
        return -1;
    }
    if (len_ == 0 || len_ < how_you_want)
    {
        return 0;
    }
    else
    {
        memcpy(dest, data_.data(), how_you_want);
        memmove(data_.data(), data_.data() + how_you_want, len_ - how_you_want);
        len_ -= how_you_want;
        return how_you_want;
    }
}

unsigned char* audio_buffer::get_writable_ptr()
{
    return data_.data() + len_;
}

void audio_buffer::update_data_len(int len)
{
    if (len < 0)
    {
        len_ = std::max(0, len_ + len);
        return;
    }
    if (static_cast<int>(data_.size()) - len_ < len)
    {
        len_ = static_cast<int>(data_.size());
        return;
    }
    len_ += len;
}
