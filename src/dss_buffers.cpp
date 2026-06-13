/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "dss_buffers.h"

#include <cstdlib>
#include <cstring>

int init_buffers(buffers_t *bufs, int bufsize, int bufnum)
{
    if (bufs == nullptr || bufsize <= 0 || bufnum <= 1)
    {
        return -1;
    }

    bufs->rear = 0;
    bufs->front = 0;
    bufs->bufnum = bufnum > MAX_BUF_NUM ? MAX_BUF_NUM : bufnum;

    auto release_allocated_buffers = [&]()
    {
        for (int i = 0; i < bufs->bufnum; i++)
        {
            free(bufs->buf[i].start);
            bufs->buf[i] = {};
        }
        bufs->bufnum = 0;
    };

    for (int i = 0; i < bufs->bufnum; i++)
    {
        bufs->buf[i].length = 0;
        bufs->buf[i].capacity = static_cast<std::size_t>(bufsize);
        bufs->buf[i].start = static_cast<char *>(calloc(1, bufsize));
        bufs->buf[i].frame_type = -1;
        bufs->buf[i].channel = -1;
        bufs->buf[i].frame_index = 0;
        if (bufs->buf[i].start == nullptr)
        {
            release_allocated_buffers();
            return -1;
        }
    }
    const int mutex_result = pthread_mutex_init(&(bufs->mutex), nullptr);
    if (mutex_result != 0)
    {
        release_allocated_buffers();
    }
    return mutex_result;
}

int free_buffers(buffers_t *bufs)
{
    if (bufs == nullptr)
    {
        return -1;
    }

    for (int i = 0; i < bufs->bufnum; i++)
    {
        if (bufs->buf[i].start != nullptr)
        {
            free(bufs->buf[i].start);
        }
        bufs->buf[i] = {};
    }
    bufs->pOnVideoData = nullptr;
    bufs->pUserData = nullptr;
    bufs->rear = 0;
    bufs->front = 0;
    bufs->bufnum = 0;
    pthread_mutex_destroy(&(bufs->mutex));
    return 0;
}

int buffers_get_data(void *data, unsigned int *length, buffers_t *bufs, int *type, int *channel, int *frame_index)
{
    if (data == nullptr || length == nullptr || bufs == nullptr || type == nullptr || channel == nullptr ||
        frame_index == nullptr || bufs->bufnum <= 0)
    {
        return -1;
    }

    int res = -1;
    pthread_mutex_lock(&(bufs->mutex));
    if (bufs->front != bufs->rear)
    {
        buffer_t &front = bufs->buf[bufs->front];
        if (*length < front.length)
        {
            res = -2;
        }
        else
        {
            *length = static_cast<unsigned int>(front.length);
            memcpy(data, front.start, front.length);
            *type = front.frame_type;
            *channel = front.channel;
            *frame_index = front.frame_index;
            front.length = 0;
            bufs->front = (bufs->front + 1) % bufs->bufnum;
            res = static_cast<int>(*length);
        }
    }
    pthread_mutex_unlock(&(bufs->mutex));
    return res;
}

int buffers_put_data(const void *data, unsigned int length, buffers_t *bufs, int type, int channel, int frame_index)
{
    if ((data == nullptr && length > 0) || bufs == nullptr || bufs->bufnum <= 0)
    {
        return -1;
    }

    int res = 0;
    pthread_mutex_lock(&(bufs->mutex));

    if (((bufs->rear + 1) % bufs->bufnum) == bufs->front)
    {
        res = -1;
    }
    else
    {
        buffer_t &rear = bufs->buf[bufs->rear];
        if (length > rear.capacity)
        {
            res = -2;
        }
        else
        {
            rear.length = length;
            rear.frame_type = type;
            rear.channel = channel;
            rear.frame_index = frame_index;
            if (length > 0)
            {
                memcpy(rear.start, data, length);
            }
            bufs->rear = (bufs->rear + 1) % bufs->bufnum;
            res = 1;
        }
    }

    pthread_mutex_unlock(&(bufs->mutex));
    return res;
}

void buffers_clear_data(buffers_t *bufs)
{
    if (bufs == nullptr || bufs->bufnum <= 0)
    {
        return;
    }

    pthread_mutex_lock(&(bufs->mutex));
    bufs->rear = 0;
    bufs->front = 0;
    for (int i = 0; i < bufs->bufnum; i++)
    {
        bufs->buf[i].length = 0;
    }
    pthread_mutex_unlock(&(bufs->mutex));
}
