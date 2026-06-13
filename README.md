# EasyAACEncoder

EasyAACEncoder is a small C/C++ audio transcoding library that converts
G.711 A-law, G.711 u-law, G.726, and raw PCM16 audio into AAC.

The project preserves the original EasyDarwin C API while using a modern C++23
implementation internally. FAAC is bundled for AAC encoding, and Kiss FFT is
bundled through the FAAC source tree.


## Features

* AAC output with ADTS headers.
* Input support for:
  * G.711 A-law
  * G.711 u-law
  * G.726 at 16, 24, 32, and 40 kbit/s
  * Raw 16-bit PCM
* Public C API suitable for C and C++ callers.
* Static and shared library builds.
* CMake-based build system.


## Requirements

* CMake 3.20 or newer.
* A C99 compiler for bundled FAAC/Kiss FFT sources.
* A C++23 compiler for the EasyAACEncoder wrapper code.
* Linux or another POSIX-like environment with `pthread` and `libm`.


## Build

Configure and build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Generated artifacts are written to `dist/`:

* `dist/libEasyAACEncoder.a`
* `dist/libEasyAACEncoder.so`
* `dist/easy_aac_encoder_sample`

Useful targets:

```sh
cmake --build build --target static
cmake --build build --target shared
cmake --build build --target sample
cmake --build build --target warnings
cmake --build build --target fmt
cmake --build build --target clean-generated
```

The `warnings` target compiles project-owned C++23 sources with
`-Wall -Wextra -Werror`. Vendored FAAC and Kiss FFT C sources are built
separately and are not forced through the project C++ warning policy.


## Sample Program

The sample executable is built as:

```sh
cmake --build build --target sample
```

Run it from the sample-data directory so relative input paths resolve:

```sh
cd examples/basic_transcoder
../../dist/easy_aac_encoder_sample
```

By default, the sample encodes `playback.pcm` to `playback.aac`. The same sample
source also includes G.711 and G.726 examples.


## Public API

include the public header:

```c
#include "EasyAACEncoderAPI.h"
```

Basic flow:

```c
InitParam init = {0};
init.ucAudioCodec = Law_PCM16;
init.ucAudioChannel = 1;
init.u32AudioSamplerate = 8000;
init.u32PCMBitSize = 16;

Easy_Handle encoder = Easy_AACEncoder_Init(init);
if (encoder == NULL) {
    /* handle init failure */
}

unsigned int out_len = 0;
int rc = Easy_AACEncoder_EncodeEx(
    encoder,
    input_data,
    input_size,
    output_data,
    output_capacity,
    &out_len
);

Easy_AACEncoder_Release(encoder);
```

Prefer `Easy_AACEncoder_EncodeEx` for new code because it receives the output
buffer capacity and can detect undersized buffers.

`Easy_AACEncoder_Encode` is still available for compatibility with existing
callers, but it cannot verify the output buffer capacity.


## Supported Codec Values

```c
enum Law
{
    Law_ULaw = 0,
    Law_ALaw = 1,
    Law_PCM16 = 2,
    Law_G726 = 3
};
```

For G.726 input, set `init.g726param.ucRateBits` to one of:

```c
enum Rate
{
    Rate16kBits = 2,
    Rate24kBits = 3,
    Rate32kBits = 4,
    Rate40kBits = 5
};
```


## Repository Layout

* `include/` - public API header.
* `src/` - EasyAACEncoder implementation and codec wrappers.
* `src/libfaac/` - bundled FAAC encoder sources.
* `src/include/` - bundled FAAC public headers.
* `src/libfaac/kiss_fft/` - bundled Kiss FFT sources used by FAAC.
* `examples/basic_transcoder/` - sample program and sample input data.


## Acknowledgements

EasyAACEncoder builds on work from:

* [EasyDarwin](http://www.easydarwin.org), which organized and developed the
  original EasyAACEncoder library and API.
* [FAAC](https://www.audiocoding.com/faac.html), the Freeware Advanced Audio
  Coder used for AAC encoding.
* [Kiss FFT](https://github.com/mborgerding/kissfft), used by the bundled FAAC
  implementation.
* ITU-T G.711 and G.726 audio codec specifications.

Review the license notices in the bundled source files before redistributing
modified builds.


## More Information

Email: [support@easydarwin.org](mailto:support@easydarwin.org)

Website: [www.EasyDarwin.org](http://www.easydarwin.org)

Copyright (c) EasyDarwin.org 2012-2019
Copyright (c) 2026 Yehor Smoliakov
