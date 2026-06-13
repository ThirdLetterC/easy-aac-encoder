/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once

#include <cstdio>

inline constexpr int DBG_MSG_BUFF_SIZE = 3000;
inline constexpr const char* FILE_NAME = "-EasyAACEncoder-";
inline constexpr bool AAC_DEBUG = false;

#define INFO_D(OUT, fmt, ...)                                                                                          \
    {                                                                                                                  \
        if (OUT)                                                                                                       \
        {                                                                                                              \
            std::printf(fmt, ##__VA_ARGS__);                                                                           \
        }                                                                                                              \
    }
