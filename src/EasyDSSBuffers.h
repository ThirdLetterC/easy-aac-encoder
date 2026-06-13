/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

#include <errno.h>

#include <string>
#include <stdlib.h>
#include <map>

using namespace std;

#define PFRAME 0x00
#define IFRAME 0x01

const int iBufLen = 1024 * 128;
const int iRecvBufLen = iBufLen * 2;
const int iMaxParamterNum = 128;
const int iBufNum = 10;
const int MAX_BUF_NUM = 50;
const int iCharBufLen = 64;
const char BoundTag[] = "\r\n";               // Frame separator marker
const int BoundTagLen = sizeof(BoundTag) - 1; // Frame separator marker length
// const char IFrameTag[]  = "HISI264I";// Recording I-frame marker
// const char PFrameTag[]  = "HISI264P";// Recording P-frame marker
// const int FrameTagLen = sizeof(IFrameTag) - 1;// Recording frame marker length

const int MaxWaitTime = 3000; // Timeout
const int MaxCameraNum = 24;  // Maximum number of cameras

const int AUDIO_BUFFER_SIZE = 960; // Size of each playback buffer block
const int MAX_AUDIO_BUF = 4;       // Notification index for the playback buffer
const int BUFFERNOTIFYSIZE = AUDIO_BUFFER_SIZE;
/*8192*/ /*192000*/ // Buffer notification position size. See the DirectSound application development quick start.
const int SAMPLE_RATE = 8000; /*44100*/ // PCM sample rate: 8k
const int N_CHANNEL = 1; /*2*/          // Number of PCM channels: mono
const int BITS_PER_SAMPLE = 16;         // Bits per sample
const int CHANNEL = 1;
const int SAMPLES_PER_SECOND = 8000;
const int SIZE_AUDIO_FRAME = 960;

#ifndef TRACE
#define TRACE printf
#endif

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
    {
        pOnVideoData = NULL;
        pUserData = NULL;
    }

} buffers_t;

int init_buffers(buffers_t *bufs, int bufsize, int bufnum);

int free_buffers(buffers_t *bufs);

int buffers_get_data(void *data, unsigned int *length, buffers_t *bufs, int *type, int *channel, int *frame_index);

int buffers_put_data(void *data, unsigned int length, buffers_t *bufs, int type, int channel, int frame_index);

void buffers_clear_data(buffers_t *bufs);

#endif
