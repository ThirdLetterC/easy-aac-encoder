/*
    Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
    Github: https://github.com/EasyDarwin
    WEChat: EasyDarwin
    Website: http://www.easydarwin.org
*/

#pragma once
#include "decode_to_pcm.h"

class G711AToPcm : public DecodeToPcmBase
{
  public:
    G711AToPcm() = default;
    ~G711AToPcm() override = default;

  public:
    unsigned short DecodeOneChar(unsigned char data) noexcept override;
};

class G711UToPcm : public DecodeToPcmBase
{
  public:
    G711UToPcm() = default;
    ~G711UToPcm() override = default;

  public:
    unsigned short DecodeOneChar(unsigned char data) noexcept override;
};
