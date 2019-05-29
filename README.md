# cppimg
[![Build Status](https://travis-ci.org/taqu/cppimg.svg?branch=master)](https://travis-ci.org/taqu/cppimg)

# Introduction
A library of loading/saveing some image formats for C++

# Usage
To create an implementation, put CPPIMG_IMPLEMENTATION like below.

    #define CPPIMG_IMPLEMENTATION
    #include "cppimg.h"

## Options
To disable using Half float convert (F16C instructions), put before "include cppimg.h"

    #define CPPIMG_DISABLE_F16C

To disable using AVX/SSE instructions, put before "include cppimg.h"

    #define CPPIMG_DISABLE_AVX

# Supported Formats

||Input|Output|Bits/Pixel|Note|
|:---|:---|:---|:---|:---|
|PPM|no|yes|8/24/32||
|BMP|yes|yes|24/32|Support only uncompressed. Not support alpha, color spaces.|
|TGA|yes|yes|24/32|Support both uncompressed and compressed.|
|PNG|yes|no|8/24/32||
|JPG|yes|no|8/24|Support only base line. Not support progressive.|
|OpenEXR|yes|yes|16/32|Support only gray, rgb, or rgba image.|

## Supported properties in Open EXR
|Property|Input|Output|
|:--|:--|:--|
|No Compression|yes|no|
|RLE|yes|no|
|ZIPS|yes|no|
|ZIP|yes|yes|
|PIZ/PXR24/B44/B44A|no|no|
|Tiled image|no|no|
|Multi-part|no|no|
|Deep Data|no|no|

# License
This is free and unencumbered software released into the public domain.
