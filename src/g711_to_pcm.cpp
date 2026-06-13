/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#include "g711_to_pcm.h"

#include "g711.h"

unsigned short G711AToPcm::DecodeOneChar(unsigned char data)
{
    return (int16_t)alaw2linear(data);
}
//-------------------------------------------
unsigned short G711UToPcm::DecodeOneChar(unsigned char data)
{
    return (int16_t)ulaw2linear(data);
}
