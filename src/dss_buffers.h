/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once

#include <pthread.h>

#include <cstddef>

inline constexpr int PFRAME = 0x00;
inline constexpr int IFRAME = 0x01;

inline constexpr int iBufLen = 1024 * 128;
inline constexpr int iRecvBufLen = iBufLen * 2;
inline constexpr int iMaxParamterNum = 128;
inline constexpr int iBufNum = 10;
inline constexpr int MAX_BUF_NUM = 50;
inline constexpr int iCharBufLen = 64;
inline constexpr char BoundTag[] = "\r\n";               // Frame separator marker
inline constexpr int BoundTagLen = sizeof(BoundTag) - 1; // Frame separator marker length
// const char IFrameTag[]  = "HISI264I";// Recording I-frame marker
// const char PFrameTag[]  = "HISI264P";// Recording P-frame marker
// const int FrameTagLen = sizeof(IFrameTag) - 1;// Recording frame marker length

inline constexpr int MaxWaitTime = 3000; // Timeout
inline constexpr int MaxCameraNum = 24;  // Maximum number of cameras

inline constexpr int AUDIO_BUFFER_SIZE = 960; // Size of each playback buffer block
inline constexpr int MAX_AUDIO_BUF = 4;       // Notification index for the playback buffer
inline constexpr int BUFFERNOTIFYSIZE = AUDIO_BUFFER_SIZE;
/*8192*/ /*192000*/ // Buffer notification position size. See the DirectSound application development quick start.
inline constexpr int SAMPLE_RATE = 8000; /*44100*/ // PCM sample rate: 8k
inline constexpr int N_CHANNEL = 1; /*2*/          // Number of PCM channels: mono
inline constexpr int BITS_PER_SAMPLE = 16;         // Bits per sample
inline constexpr int CHANNEL = 1;
inline constexpr int SAMPLES_PER_SECOND = 8000;
inline constexpr int SIZE_AUDIO_FRAME = 960;

/*!
 \brief H264 stream callback function pointer
 \param iPlayHandle Playback handle
 \param sData Data buffer
 \param iLen
 Data length
 \param iDataType Data type: 0 - live stream, 1 - recorded stream, 2 - locally captured audio stream, 3 -
 audio stream sent by the device
*/
typedef void (*fVideoDataCallBack)(int iPlayHandle, char *sData, int iLen, int iDataType, void *pUserData);

/*!
 \brief Alarm message callback function pointer
 \param pAlarmInfo Pointer to the alarm information T_JD_AlarmInfo
 * structure
 \param pUserData User data
*/
typedef void (*fMessageCallBack)(void *pAlarmInfo, void *pUserData);

typedef struct
{
    char *start;
    std::size_t capacity;
    size_t length;
    int frame_type;
    int frame_index;
    int channel;
} buffer_t;

typedef struct _buffers_t
{
    int rear;
    int front;
    int bufnum;
    int fps;
    pthread_mutex_t mutex;
    buffer_t buf[MAX_BUF_NUM];
    fVideoDataCallBack pOnVideoData; // Data callback function pointer
    void *pUserData;                 // Callback user data

    _buffers_t()
        : rear(0), front(0), bufnum(0), fps(0), pOnVideoData(nullptr), pUserData(nullptr)
    {
        for (auto &item : buf)
        {
            item = {};
        }
    }

} buffers_t;

int init_buffers(buffers_t *bufs, int bufsize, int bufnum);

int free_buffers(buffers_t *bufs);

int buffers_get_data(void *data, unsigned int *length, buffers_t *bufs, int *type, int *channel, int *frame_index);

int buffers_put_data(const void *data, unsigned int length, buffers_t *bufs, int type, int channel, int frame_index);

void buffers_clear_data(buffers_t *bufs);
