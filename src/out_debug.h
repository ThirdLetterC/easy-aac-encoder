/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#ifndef _OUTDEBUG_H
#define _OUTDEBUG_H

#define DBG_MSG_BUFF_SIZE 3000
#define FILE_NAME "-EasyAACEncoder-"

#include <stdio.h>

#define INFO_D(OUT, fmt, ...)                                                                                          \
    {                                                                                                                  \
        if (OUT)                                                                                                       \
        {                                                                                                              \
            printf(fmt, ##__VA_ARGS__);                                                                                \
        }                                                                                                              \
    }

#define AAC_DEBUG 0

#endif
