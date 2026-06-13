#include "audio_buffer.h"

#include <algorithm>
#include <cstddef>
#include <limits>

namespace
{
constexpr std::size_t DefaultBufferCapacity = 1024U * 128U;
}

audio_buffer::audio_buffer()
    : data_(DefaultBufferCapacity)
{
}

int audio_buffer::write_data(const void* data, int len)
{
    if (data == nullptr || len < 0)
    {
        return -1;
    }
    const auto write_len = static_cast<std::size_t>(len);
    if (data_.size() - len_ < write_len)
    {
        return -1;
    }
    if (write_len > 0)
    {
        const auto* first = static_cast<const unsigned char*>(data);
        std::copy_n(first, write_len, data_.data() + len_);
        len_ += write_len;
        return len;
    }
    return 0;
}

int audio_buffer::get_data(unsigned char* dest, int how_you_want)
{
    if (dest == nullptr || how_you_want <= 0)
    {
        return -1;
    }
    const auto requested = static_cast<std::size_t>(how_you_want);
    if (len_ == 0 || len_ < requested)
    {
        return 0;
    }

    std::copy_n(data_.data(), requested, dest);
    std::move(data_.begin() + requested, data_.begin() + static_cast<std::ptrdiff_t>(len_), data_.begin());
    len_ -= requested;
    return how_you_want;
}

unsigned char* audio_buffer::get_writable_ptr()
{
    return data_.data() + len_;
}

void audio_buffer::update_data_len(int len)
{
    if (len < 0)
    {
        const auto shrink_by = static_cast<std::size_t>(-len);
        len_ = shrink_by >= len_ ? 0 : len_ - shrink_by;
        return;
    }
    const auto grow_by = static_cast<std::size_t>(len);
    if (data_.size() - len_ < grow_by)
    {
        len_ = data_.size();
        return;
    }
    len_ += grow_by;
}
