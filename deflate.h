#ifndef INC_CPPZ_H_
#define INC_CPPZ_H_
/* zlib.h -- interface of the 'zlib' general purpose compression library
version 1.2.11, January 15th, 2017

Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Jean-loup Gailly        Mark Adler
jloup@gzip.org          madler@alumni.caltech.edu


The data format used by the zlib library is described by RFCs (Request for
Comments) 1950 to 1952 in the files http://tools.ietf.org/html/rfc1950
(zlib format), rfc1951 (deflate format) and rfc1952 (gzip format).
*/
/*
The above is the original zlib's license.
This is a portion of deflate/inflate codes extracted from the zlib library.
So, this codes is published under zlib/libpng license, modified by
Copyright (C) 2017 Takuro Sakai
*/
/**
@file deflate.h

USAGE:
  Put '#define CPPZ_IMPLEMENTATION' before including this file to create the implementation.

*/
#include <cassert>
#include <cstdint>
#include <malloc.h>
#include <memory.h>

namespace cppz
{
#ifdef __cplusplus
#   if 201103L<=__cplusplus || 1900<=_MSC_VER
#       define CPPZ_CPP11 1
#   endif
#endif

#ifdef __cplusplus
#   ifdef CPPZ_CPP11
#       define CPPZ_NULL nullptr
#   else
#       define CPPZ_NULL 0
#   endif
#else
#   define CPPZ_NULL (void*)0
#endif

#ifndef CPPZ_TYPES
#define CPPZ_TYPES
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;
    typedef bool boolean;

    typedef ::size_t size_t;
#endif

#ifndef CPPZ_MALLOC
#define CPPZ_MALLOC(size) malloc(size)
#endif

#ifndef CPPZ_FREE
#define CPPZ_FREE(ptr) free(ptr)
#endif

#ifndef CPPZ_ASSERT
#define CPPZ_ASSERT(exp) assert(exp)
#endif

    typedef void*(*cppz_alloc_func)(size_t size, void* user);
    typedef void(*cppz_free_func)(void* ptr, void* user);

    enum CPPZ_FLUSH
    {
        CPPZ_NO_FLUSH =0,
        CPPZ_PARTIAL_FLUSH =1,
        CPPZ_SYNC_FLUSH =2,
        CPPZ_FULL_FLUSH =3,
        CPPZ_FINISH =4,
        CPPZ_BLOCK =5,
        CPPZ_TREES =6,
    };

    enum CPPZ_STATUS
    {
        CPPZ_OK =0,
        CPPZ_STREAM_END =1,
        CPPZ_NEED_DICT =2,
        CPPZ_ERRNO = -1,
        CPPZ_STREAM_ERROR = -2,
        CPPZ_DATA_ERROR = -3,
        CPPZ_MEM_ERROR = -4,
        CPPZ_BUF_ERROR = -5,
        CPPZ_VERSION_ERROR = -6,
    };

    enum CPPZ_MODE
    {
        CPPZ_MODE_HEAD = 16180,
        CPPZ_MODE_FLAGS,
        CPPZ_MODE_TIME,
        CPPZ_MODE_OS,
        CPPZ_MODE_EXLEN,
        CPPZ_MODE_EXTRA,
        CPPZ_MODE_NAME,
        CPPZ_MODE_COMMENT,
        CPPZ_MODE_HCRC,
        CPPZ_MODE_DICTID,
        CPPZ_MODE_DICT,
        CPPZ_MODE_TYPE,
        CPPZ_MODE_TYPEDO,
        CPPZ_MODE_STORED,
        CPPZ_MODE_COPY_,
        CPPZ_MODE_COPY,
        CPPZ_MODE_TABLE,
        CPPZ_MODE_LENLENS,
        CPPZ_MODE_CODELENS,
        CPPZ_MODE_LEN_,
        CPPZ_MODE_LEN,
        CPPZ_MODE_LENEXT,
        CPPZ_MODE_DIST,
        CPPZ_MODE_DISTEXT,
        CPPZ_MODE_MATCH,
        CPPZ_MODE_LIT,
        CPPZ_MODE_CHECK,
        CPPZ_MODE_LENGTH,
        CPPZ_MODE_DONE,
        CPPZ_MODE_BAD,
        CPPZ_MODE_MEM,
        CPPZ_MODE_SYNC,
    };


    static const u32 CPPZ_DEFLATED = 8;
    static const u32 CPPZ_PRESET_DICT = 0x20U;

    static const u32 CPPZ_FILTERED = 1;
    static const u32 CPPZ_HUFFMAN_ONLY = 2;
    static const u32 CPPZ_RLE = 3;
    static const u32 CPPZ_FIXED = 4;
    static const u32 CPPZ_DEFAULT_STRATEGY = 0;

    static const s32 CPPZ_DEFAULT_LEVEL = 6;
    static const s32 CPPZ_DEFAULT_MEM_LEVEL = 8;
    static const s32 CPPZ_DEFAULT_WINDOW_BITS = 15;

    static const s32 CPPZ_STORED_BLOCK = 0;
    static const s32 CPPZ_STATIC_TREES = 1;
    static const s32 CPPZ_DYN_TREES = 2;

    static const s32 MIN_MATCH = 3;
    static const s32 MAX_MATCH = 258;
    static const s32 MIN_LOOKAHEAD = (MIN_MATCH+MAX_MATCH+1);
    static const s32 WIN_INIT = MAX_MATCH;

    static const s32 ENOUGH_LENS = 852;
    static const s32 ENOUGH_DISTS = 592;
    static const s32 ENOUGH = (ENOUGH_LENS+ENOUGH_DISTS);

    struct DeflateContext;
    struct InflateContext;

    struct Stream
    {
        const u8* next_in_;
        u32 avail_in_;
        u64 total_in_;

        u8* next_out_;
        u32 avail_out_;
        u64 total_out_;

        u32 adler32_;
        cppz_alloc_func alloc_func_;
        cppz_free_func free_func_;
        void* user_;

        union{
            DeflateContext* deflate_;
            InflateContext* inflate_;
        };
    };

    s32 cppz_deflateReset(Stream& stream);
    s32 cppz_deflateInit(Stream& stream, cppz_alloc_func alloc_func=CPPZ_NULL, cppz_free_func free_func=CPPZ_NULL, void* user=CPPZ_NULL);
    s32 cppz_deflate(Stream& stream, s32 flush);
    s32 cppz_deflateEnd(Stream& stream);

    s32 cppz_inflateReset(Stream& stream);
    s32 cppz_inflateInit(Stream& stream, cppz_alloc_func alloc_func=CPPZ_NULL, cppz_free_func free_func=CPPZ_NULL, void* user=CPPZ_NULL);
    s32 cppz_inflate(Stream& stream, s32 flush);
    s32 cppz_inflateEnd(Stream& stream);

#ifdef CPPZ_IMPLEMENTATION

namespace
{
    //--------------------------------------------------------------------
    //---
    //--- Tree
    //---
    //--------------------------------------------------------------------
    //number of length codes, not counting the special END_BLOCK code
    static const s32 LENGTH_CODES = 29;
    //number of literal bytes 0..255
    static const s32 LITERALS = 256;
    //number of Literal or Length codes, including the END_BLOCK code
    static const s32 L_CODES = (LITERALS+1+LENGTH_CODES);
    //number of distance codes
    static const s32 D_CODES = 30;
    //number of codes used to transfer the bit lengths
    static const s32 BL_CODES = 19;

    //maximum heap size
    static const s32 HEAP_SIZE = (2*L_CODES+1);

    //all codes must not exceed MAX_BITS bits
    static const s32 MAX_BITS = 15;

    //see definition of array dist_code below
    static const s32 DIST_CODE_LEN = 512;

    //Bit length codes must not exceed MAX_BL_BITS bits
    static const s32 MAX_BL_BITS = 7;

    //end of block literal code
    static const s32 END_BLOCK = 256;

    //repeat previous bit length 3-6 times (2 bits of repeat count)
    static const s32 REP_3_6 = 16;

    //repeat a zero length 3-10 times  (3 bits of repeat count)
    static const s32 REPZ_3_10 = 17;

    //repeat a zero length 11-138 times  (7 bits of repeat count)
    static const s32 REPZ_11_138 = 18;

    static const s32 SMALLEST = 1;

    struct ct_data
    {
        union {
            u16  freq;       /* frequency count */
            u16  code;       /* bit string */
        } fc;
        union {
            u16  dad;        /* father node in Huffman tree */
            u16  len;        /* length of bit string */
        } dl;
    };
#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

    const ct_data static_ltree[L_CODES+2] = {
        {{ 12},{  8}}, {{140},{  8}}, {{ 76},{  8}}, {{204},{  8}}, {{ 44},{  8}},
        {{172},{  8}}, {{108},{  8}}, {{236},{  8}}, {{ 28},{  8}}, {{156},{  8}},
        {{ 92},{  8}}, {{220},{  8}}, {{ 60},{  8}}, {{188},{  8}}, {{124},{  8}},
        {{252},{  8}}, {{  2},{  8}}, {{130},{  8}}, {{ 66},{  8}}, {{194},{  8}},
        {{ 34},{  8}}, {{162},{  8}}, {{ 98},{  8}}, {{226},{  8}}, {{ 18},{  8}},
        {{146},{  8}}, {{ 82},{  8}}, {{210},{  8}}, {{ 50},{  8}}, {{178},{  8}},
        {{114},{  8}}, {{242},{  8}}, {{ 10},{  8}}, {{138},{  8}}, {{ 74},{  8}},
        {{202},{  8}}, {{ 42},{  8}}, {{170},{  8}}, {{106},{  8}}, {{234},{  8}},
        {{ 26},{  8}}, {{154},{  8}}, {{ 90},{  8}}, {{218},{  8}}, {{ 58},{  8}},
        {{186},{  8}}, {{122},{  8}}, {{250},{  8}}, {{  6},{  8}}, {{134},{  8}},
        {{ 70},{  8}}, {{198},{  8}}, {{ 38},{  8}}, {{166},{  8}}, {{102},{  8}},
        {{230},{  8}}, {{ 22},{  8}}, {{150},{  8}}, {{ 86},{  8}}, {{214},{  8}},
        {{ 54},{  8}}, {{182},{  8}}, {{118},{  8}}, {{246},{  8}}, {{ 14},{  8}},
        {{142},{  8}}, {{ 78},{  8}}, {{206},{  8}}, {{ 46},{  8}}, {{174},{  8}},
        {{110},{  8}}, {{238},{  8}}, {{ 30},{  8}}, {{158},{  8}}, {{ 94},{  8}},
        {{222},{  8}}, {{ 62},{  8}}, {{190},{  8}}, {{126},{  8}}, {{254},{  8}},
        {{  1},{  8}}, {{129},{  8}}, {{ 65},{  8}}, {{193},{  8}}, {{ 33},{  8}},
        {{161},{  8}}, {{ 97},{  8}}, {{225},{  8}}, {{ 17},{  8}}, {{145},{  8}},
        {{ 81},{  8}}, {{209},{  8}}, {{ 49},{  8}}, {{177},{  8}}, {{113},{  8}},
        {{241},{  8}}, {{  9},{  8}}, {{137},{  8}}, {{ 73},{  8}}, {{201},{  8}},
        {{ 41},{  8}}, {{169},{  8}}, {{105},{  8}}, {{233},{  8}}, {{ 25},{  8}},
        {{153},{  8}}, {{ 89},{  8}}, {{217},{  8}}, {{ 57},{  8}}, {{185},{  8}},
        {{121},{  8}}, {{249},{  8}}, {{  5},{  8}}, {{133},{  8}}, {{ 69},{  8}},
        {{197},{  8}}, {{ 37},{  8}}, {{165},{  8}}, {{101},{  8}}, {{229},{  8}},
        {{ 21},{  8}}, {{149},{  8}}, {{ 85},{  8}}, {{213},{  8}}, {{ 53},{  8}},
        {{181},{  8}}, {{117},{  8}}, {{245},{  8}}, {{ 13},{  8}}, {{141},{  8}},
        {{ 77},{  8}}, {{205},{  8}}, {{ 45},{  8}}, {{173},{  8}}, {{109},{  8}},
        {{237},{  8}}, {{ 29},{  8}}, {{157},{  8}}, {{ 93},{  8}}, {{221},{  8}},
        {{ 61},{  8}}, {{189},{  8}}, {{125},{  8}}, {{253},{  8}}, {{ 19},{  9}},
        {{275},{  9}}, {{147},{  9}}, {{403},{  9}}, {{ 83},{  9}}, {{339},{  9}},
        {{211},{  9}}, {{467},{  9}}, {{ 51},{  9}}, {{307},{  9}}, {{179},{  9}},
        {{435},{  9}}, {{115},{  9}}, {{371},{  9}}, {{243},{  9}}, {{499},{  9}},
        {{ 11},{  9}}, {{267},{  9}}, {{139},{  9}}, {{395},{  9}}, {{ 75},{  9}},
        {{331},{  9}}, {{203},{  9}}, {{459},{  9}}, {{ 43},{  9}}, {{299},{  9}},
        {{171},{  9}}, {{427},{  9}}, {{107},{  9}}, {{363},{  9}}, {{235},{  9}},
        {{491},{  9}}, {{ 27},{  9}}, {{283},{  9}}, {{155},{  9}}, {{411},{  9}},
        {{ 91},{  9}}, {{347},{  9}}, {{219},{  9}}, {{475},{  9}}, {{ 59},{  9}},
        {{315},{  9}}, {{187},{  9}}, {{443},{  9}}, {{123},{  9}}, {{379},{  9}},
        {{251},{  9}}, {{507},{  9}}, {{  7},{  9}}, {{263},{  9}}, {{135},{  9}},
        {{391},{  9}}, {{ 71},{  9}}, {{327},{  9}}, {{199},{  9}}, {{455},{  9}},
        {{ 39},{  9}}, {{295},{  9}}, {{167},{  9}}, {{423},{  9}}, {{103},{  9}},
        {{359},{  9}}, {{231},{  9}}, {{487},{  9}}, {{ 23},{  9}}, {{279},{  9}},
        {{151},{  9}}, {{407},{  9}}, {{ 87},{  9}}, {{343},{  9}}, {{215},{  9}},
        {{471},{  9}}, {{ 55},{  9}}, {{311},{  9}}, {{183},{  9}}, {{439},{  9}},
        {{119},{  9}}, {{375},{  9}}, {{247},{  9}}, {{503},{  9}}, {{ 15},{  9}},
        {{271},{  9}}, {{143},{  9}}, {{399},{  9}}, {{ 79},{  9}}, {{335},{  9}},
        {{207},{  9}}, {{463},{  9}}, {{ 47},{  9}}, {{303},{  9}}, {{175},{  9}},
        {{431},{  9}}, {{111},{  9}}, {{367},{  9}}, {{239},{  9}}, {{495},{  9}},
        {{ 31},{  9}}, {{287},{  9}}, {{159},{  9}}, {{415},{  9}}, {{ 95},{  9}},
        {{351},{  9}}, {{223},{  9}}, {{479},{  9}}, {{ 63},{  9}}, {{319},{  9}},
        {{191},{  9}}, {{447},{  9}}, {{127},{  9}}, {{383},{  9}}, {{255},{  9}},
        {{511},{  9}}, {{  0},{  7}}, {{ 64},{  7}}, {{ 32},{  7}}, {{ 96},{  7}},
        {{ 16},{  7}}, {{ 80},{  7}}, {{ 48},{  7}}, {{112},{  7}}, {{  8},{  7}},
        {{ 72},{  7}}, {{ 40},{  7}}, {{104},{  7}}, {{ 24},{  7}}, {{ 88},{  7}},
        {{ 56},{  7}}, {{120},{  7}}, {{  4},{  7}}, {{ 68},{  7}}, {{ 36},{  7}},
        {{100},{  7}}, {{ 20},{  7}}, {{ 84},{  7}}, {{ 52},{  7}}, {{116},{  7}},
        {{  3},{  8}}, {{131},{  8}}, {{ 67},{  8}}, {{195},{  8}}, {{ 35},{  8}},
        {{163},{  8}}, {{ 99},{  8}}, {{227},{  8}}
    };

    const ct_data static_dtree[D_CODES] = {
        {{ 0},{ 5}}, {{16},{ 5}}, {{ 8},{ 5}}, {{24},{ 5}}, {{ 4},{ 5}},
        {{20},{ 5}}, {{12},{ 5}}, {{28},{ 5}}, {{ 2},{ 5}}, {{18},{ 5}},
        {{10},{ 5}}, {{26},{ 5}}, {{ 6},{ 5}}, {{22},{ 5}}, {{14},{ 5}},
        {{30},{ 5}}, {{ 1},{ 5}}, {{17},{ 5}}, {{ 9},{ 5}}, {{25},{ 5}},
        {{ 5},{ 5}}, {{21},{ 5}}, {{13},{ 5}}, {{29},{ 5}}, {{ 3},{ 5}},
        {{19},{ 5}}, {{11},{ 5}}, {{27},{ 5}}, {{ 7},{ 5}}, {{23},{ 5}}
    };

    const u8 _dist_code[DIST_CODE_LEN] = {
        0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
        8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
        11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
        18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
        23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
        24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
        26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
        26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
        27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
        27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
        29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
        29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
        29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
    };

    const u8 _length_code[MAX_MATCH-MIN_MATCH+1]= {
        0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
        13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
        17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
        19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
        22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
        23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
        24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
        25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
        25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
        26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
        26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
        27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
    };

    const s32 base_length[LENGTH_CODES] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
        64, 80, 96, 112, 128, 160, 192, 224, 0
    };

    const s32 base_dist[D_CODES] = {
        0,     1,     2,     3,     4,     6,     8,    12,    16,    24,
        32,    48,    64,    96,   128,   192,   256,   384,   512,   768,
        1024,  1536,  2048,  3072,  4096,  6144,  8192, 12288, 16384, 24576
    };

    struct StaticTree
    {
        const ct_data* staticTree_;  // static tree or NULL
        const s32* extraBits_;      // extra bits for each code or NULL
        s32 extraBase_;          // base index for extra_bits
        s32 elements_;               // max number of elements in the tree
        s32 maxLength_;          // max bit length for the codes
    };

    //extra bits for each length code
    const s32 extra_lbits[LENGTH_CODES] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};

    //extra bits for each distance code
    const s32 extra_dbits[D_CODES] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

    //extra bits for each bit length code
    const s32 extra_blbits[BL_CODES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};

    const u8 bl_order[BL_CODES] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};

    const StaticTree  static_l_desc = {static_ltree, extra_lbits, LITERALS+1, L_CODES, MAX_BITS};

    const StaticTree  static_d_desc = {static_dtree, extra_dbits, 0,          D_CODES, MAX_BITS};

    const StaticTree  static_bl_desc = {CPPZ_NULL, extra_blbits, 0,   BL_CODES, MAX_BL_BITS};

    /* Mapping from a distance to a distance code. dist is the distance - 1 and
    * must not have side effects. _dist_code[256] and _dist_code[257] are never
    * used.
    */
    inline u8 d_code(u16 dist)
    {
        return dist<256? _dist_code[dist] : _dist_code[256+(dist>>7)];
    }

    struct Tree
    {
        ct_data* dynTree_;
        s32 maxCode_;
        const StaticTree* static_;
    };

    struct TreeContext
    {
        ct_data dyn_ltree_[HEAP_SIZE]; //literal and length tree
        ct_data dyn_dtree_[2*D_CODES+1]; //distance tree
        ct_data bl_tree_[2*BL_CODES+1];  //Huffman tree for bit lengths

        Tree literal_tree_; //literal tree
        Tree distance_tree_; //distance tree
        Tree bit_length_tree_; //bit length tree

        u16 bl_count_[MAX_BITS+1];

        s32 heap_[HEAP_SIZE];
        s32 heap_len_;
        s32 heap_max_;

        u8 depth_[HEAP_SIZE]; //Depth of each subtree used as tie breaker for trees of equal frequency

        u16 bi_buf_;
        s32 bi_valid_;

        u32 last_lit_; //running index in l_buf

        u32 opt_len_; //bit length of current block with optimal trees
        u32 static_len_; //bit length of current block with static trees
        u32 matches_; //number of string matches in current block
    };

    void init_block(TreeContext& context)
    {
        for(s32 i=0; i<L_CODES; ++i){
            context.dyn_ltree_[i].Freq = 0;
        }
        for(s32 i=0; i<D_CODES; ++i){
            context.dyn_dtree_[i].Freq = 0;
        }
        for(s32 i=0; i<BL_CODES; ++i){
            context.bl_tree_[i].Freq = 0;
        }
        context.dyn_ltree_[END_BLOCK].Freq = 1;
        context.opt_len_ = 0;
        context.static_len_ = 0;
        context.last_lit_ = 0;
        context.matches_ = 0;
    }

    void tree_init(TreeContext& context)
    {
        context.literal_tree_.dynTree_ = context.dyn_ltree_;
        context.literal_tree_.static_ = &static_l_desc;

        context.distance_tree_.dynTree_ = context.dyn_dtree_;
        context.distance_tree_.static_ = &static_d_desc;

        context.bit_length_tree_.dynTree_ = context.bl_tree_;
        context.bit_length_tree_.static_ = &static_bl_desc;

        context.bi_buf_ = 0;
        context.bi_valid_ = 0;

        //Initialize the first block of the first file:
        init_block(context);
    }


    inline bool cmp_less(ct_data* tree, s32 n, s32 m, const u8* depth)
    {
        return tree[n].Freq<tree[m].Freq || (tree[n].Freq == tree[m].Freq && depth[n]<=depth[m]);
    }

    /**
    Restore the heap property by moving down the tree starting at node k,
    exchanging a node with the smallest of its two sons if necessary, stopping
    when the heap property is re-established (each father smaller than its two sons).
    */
    void pqdownheap(TreeContext& context, ct_data* tree, s32 k)
    {
        s32 v = context.heap_[k];
        s32 j = k<<1;
        while(j<=context.heap_len_){
            if(j<context.heap_len_ && cmp_less(tree, context.heap_[j+1], context.heap_[j], context.depth_)){
                ++j;
            }
            if(cmp_less(tree, v, context.heap_[j], context.depth_)){
                break;
            }
            context.heap_[k] = context.heap_[j];
            k = j;
            j<<=1;
        }
        context.heap_[k] = v;
    }

    /**
    Remove the smallest element from the heap and recreate the heap with one less element. Updates heap and heap_len.
    */
    void pqremove(TreeContext& context, ct_data* tree, s32& top)
    {
        top = context.heap_[SMALLEST];
        context.heap_[SMALLEST] = context.heap_[context.heap_len_--];
        pqdownheap(context, tree, SMALLEST);
    }

    /**
    @brief Compute the optimal bit lengths for a tree and update the total bit length for the current block.
    */
    void gen_bitlen(TreeContext& context, Tree& desc)
    {
        ct_data* tree = desc.dynTree_;
        const ct_data* stree = desc.static_->staticTree_;

        for(s32 bits=0; bits<=MAX_BITS; ++bits){
            context.bl_count_[bits] = 0;
        }

        //
        tree[context.heap_[context.heap_max_]].Len = 0; //root of heap

        s32 max_code = desc.maxCode_;
        const s32* extra = desc.static_->extraBits_;
        s32 base = desc.static_->extraBase_;
        s32 max_length = desc.static_->maxLength_;
        s32 overflow = 0; //number of elements with bit length too large
        for(s32 h=context.heap_max_+1; h<HEAP_SIZE; ++h){
            s32 n = context.heap_[h];
            s32 bits = tree[tree[n].Dad].Len + 1;
            if(max_length<bits){
                bits = max_length;
                ++overflow;
            }
            tree[n].Len = static_cast<u16>(bits);
            if(max_code<n){
                continue;
            }
            ++context.bl_count_[bits];
            s32 xbits = 0;
            if(base<=n){
                xbits = extra[n-base];
            }
            u32 freq = tree[n].Freq;
            context.opt_len_ += freq * (bits + xbits);
            if(CPPZ_NULL != stree){
                context.static_len_ += freq * (stree[n].Len + xbits);
            }
        } //for(s32 h=context
        if(overflow<=0){
            return;
        }

        //Find the first bit length which could increase
        do{
            s32 bits = max_length-1;
            while(context.bl_count_[bits]<=0){
                --bits;
            }
            --context.bl_count_[bits]; //move one leaf down the tree
            context.bl_count_[bits+1] += 2; //move one overflow item as its brother
            --context.bl_count_[max_length];
            overflow -= 2;
        }while(0<overflow);

        // Now recompute all bit lengths, scanning in increasing frequency. h is still equal to HEAP_SIZE.
        s32 h = HEAP_SIZE;
        for(s32 bits = max_length; 0<bits; --bits){
            s32 n = context.bl_count_[bits];
            while(0<n){
                s32 m = context.heap_[--h];
                if(max_code<m){
                    continue;
                }
                if(tree[m].Len != bits){
                    context.opt_len_ += (bits - tree[m].Len) * tree[m].Freq;
                    tree[m].Len = static_cast<u16>(bits);
                }
                --n;
            }
        }//for(s32 bits
    }

    /**
    @brief Reverse the first len bits of a code, using straightforward code (a faster method would use a table)
    @param code ... the value to invert
    @param len ... its bit length
    */
    u32 bi_reverse(u32 code, s32 len)
    {
        u32 res = 0;
        do{
            res |= code & 1U;
            code >>= 1;
            res <<= 1;
        } while(0<--len);
        return res >> 1;
    }

    /**
    @brief Generate the codes for a given tree and bit counts (which need not be optimal).
    */
    void gen_codes(ct_data* tree, s32 max_code, u16* bl_count)
    {
        u16 next_code[MAX_BITS+1]; //next code value for each bit length
        u32 code = 0;
        for(s32 bits=1; bits<=MAX_BITS; ++bits){
            code = (code + bl_count[bits-1])<<1;
            next_code[bits] = static_cast<u16>(code);
        }
        for(s32 n=0; n<max_code; ++n){
            s32 len = tree[n].Len;
            if(len<=0){
                continue;
            }
            tree[n].Code = static_cast<u16>(bi_reverse(next_code[len], len));
            ++next_code[len];
        }
    }
    /**
    @brief Scan a literal or distance tree to determine the frequencies of the codes in the bit length tree.
    */
    void scan_tree(TreeContext& context, ct_data* tree, s32 max_code)
    {
        s32 prevlen = -1;
        s32 nextlen = tree[0].Len;
        s32 max_count = 7;
        s32 min_count = 4;
        if(nextlen<=0){
            max_count = 138;
            min_count = 3;
        }
        tree[max_code+1].Len = 0xFFFFU;

        s32 count = 0;
        for(s32 n=0; n<=max_code; ++n){
            s32 curlen = nextlen;
            nextlen = tree[n+1].Len;
            if(++count<max_count && curlen==nextlen){
                continue;
            }else if(count<min_count){
                context.bl_tree_[curlen].Freq += static_cast<u16>(count);
            }else if(0 != curlen){
                if(prevlen != curlen){
                    ++context.bl_tree_[curlen].Freq;
                }
                ++context.bl_tree_[REP_3_6].Freq;
            }else if(count<=10){
                ++context.bl_tree_[REPZ_3_10].Freq;
            }else{
                ++context.bl_tree_[REPZ_11_138].Freq;
            }

            count = 0;
            prevlen = curlen;
            if(0 == nextlen){
                max_count = 138;
                min_count = 3;
            }else if(curlen == nextlen){
                max_count = 6;
                min_count = 3;
            }else{
                max_count = 7;
                min_count = 4;
            }
        }//for(s32 n=0
    }

    /**
    @brief Construct one Huffman tree and assigns the code bit strings and length. Update the total bit length for the current block.
    */
    void build_tree(TreeContext& context, Tree& desc)
    {
        ct_data* tree = desc.dynTree_;
        const ct_data* stree = desc.static_->staticTree_;
        s32 elems = desc.static_->elements_;

        context.heap_len_ = 0;
        context.heap_max_ = HEAP_SIZE;

        s32 max_code = -1; //largest code with non zero frequency
        for(s32 n=0; n<elems; ++n){
            if(0 != tree[n].Freq){
                context.heap_[++(context.heap_len_)] = max_code = n;
                context.depth_[n] = 0;
            }else{
                tree[n].Len = 0;
            }
        }

        while(context.heap_len_<2){
            s32 node; //new node being created
            node = context.heap_[++(context.heap_len_)] = (max_code < 2) ? ++max_code : 0;
            tree[node].Freq = 1;
            context.depth_[node] = 0;
            //node is 0 or 1 so it does not have extra bits
            --context.opt_len_;
            if(CPPZ_NULL != stree){
                context.static_len_ -= stree[node].Len;
            }
        }
        desc.maxCode_ = max_code;

        // The elements heap[heap_len/2+1 .. heap_len] are leaves of the tree, establish sub-heaps of increasing lengths:
        for(s32 n=context.heap_len_/2; 1<=n; --n){
            pqdownheap(context, tree, n);
        }
        // Construct the Huffman tree by repeatedly combining the least two frequent nodes.
        s32 node = elems;
        do{
            s32 n;
            pqremove(context, tree, n);
            s32 m = context.heap_[SMALLEST];
            context.heap_[--context.heap_max_] = n;
            context.heap_[--context.heap_max_] = m;
            tree[node].Freq = tree[n].Freq + tree[m].Freq;
            context.depth_[node] = ((context.depth_[m]<=context.depth_[n])? context.depth_[n] : context.depth_[m])+1;
            tree[n].Dad = tree[m].Dad = static_cast<u16>(node);
            context.heap_[SMALLEST] = node++;
            pqdownheap(context, tree, SMALLEST);
        }while(2<=context.heap_len_);

        context.heap_[--context.heap_max_] = context.heap_[SMALLEST];

        // At this point, the fields freq and dad are set. We can now generate the bit lengths.
        gen_bitlen(context, desc);

        // The field len is now set, we can generate the bit codes
        gen_codes(tree, max_code, context.bl_count_);
    }

    /**
    @brief Construct the Huffman tree for the bit lengths and return the index in bl_order of the last bit length code to send.
    */
    s32 build_bl_tree(TreeContext& context)
    {
        scan_tree(context, context.dyn_ltree_, context.literal_tree_.maxCode_);
        scan_tree(context, context.dyn_dtree_, context.distance_tree_.maxCode_);

        //Build the bit length tree
        build_tree(context, context.bit_length_tree_);
        s32 max_blindex;
        for(max_blindex=BL_CODES-1; 3<=max_blindex; --max_blindex){
            if(0 != context.bl_tree_[bl_order[max_blindex]].Len){
                break;
            }
        }
        context.opt_len_ += 3*(static_cast<u32>(max_blindex)+1) + 5+5+4;
        return max_blindex;
    }
}

    static const s32 CPPZ_DICTIONARY_SIZE = 32768;
    static const s32 CPPZ_PENDING_SIZE = 65536;
    static const s32 CPPZ_TREE_BUFF_SIZE = 16384;
    struct DeflateContext
    {
        s32 state_;
        s32 last_flush_;

        s32 wrap_;
        u32 w_bits_;
        u32 w_size_;
        u32 window_size_;
        u32 w_mask_;
        u32 ins_h_;
        u32 hash_size_;
        u32 hash_bits_;
        u32 hash_mask_;
        u32 hash_shift_;

        u8 window_[CPPZ_DICTIONARY_SIZE*2];
        u16 prev_[CPPZ_DICTIONARY_SIZE];
        u16 head_[CPPZ_DICTIONARY_SIZE];
        u32 pending_;
        u8 pending_buff_[CPPZ_PENDING_SIZE];
        u8* pendingOut_;

        u32 lit_bufsize_;
        u16 dbuff_[CPPZ_TREE_BUFF_SIZE];
        u8 lbuff_[CPPZ_TREE_BUFF_SIZE];

        TreeContext treeContext_;

        s32 level_;
        s32 strategy_;

        u32 good_match_;
        u32 max_lazy_match_;
        u32 nice_match_;
        u32 max_chain_length_;

        s32 block_start_;

        u32 match_length_;
        u32 prev_match_;
        s32 match_available_;
        u32 strstart_;
        u32 match_start_;
        u32 lookahead_;
        
        u32 prev_length_;

        u32 insert_; //bytes at end of window left to insert

        u32 high_water_;
    };

    struct CODE
    {
        u8 op_;
        u8 bits_;
        u16 val_;
    };

    struct InflateContext
    {
        s32 mode_;
        s32 last_;
        s32 wrap_;
        
        s32 havedict_;
        s32 flags_;
        u32 dmax_;
        u32 check_;
        u32 total_;

        u32 wbits_;
        u32 wsize_;
        u32 whave_;
        u32 wnext_;
        u8 window_[1<<15];

        u32 hold_;
        u32 bits_;
        u32 length_;
        u32 offset_;

        u32 extra_;

        const CODE* lencode_;
        const CODE* distcode_;

        u32 lenbits_;
        u32 distbits_;

        u32 ncode_;
        u32 nlen_;
        u32 ndist_;
        u32 have_;

        CODE* next_;
        u16 lens_[320];
        u16 work_[288];
        CODE codes_[ENOUGH];
        s32 sane_;
        s32 back_;
        u32 was_;
    };

namespace
{

    /**
     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
   return the updated checksum.  If buf is Z_NULL, this function returns the
   required initial value for the checksum.

     An Adler-32 checksum is almost as reliable as a CRC-32 but can be computed
   much faster.

   Usage example:

     uLong adler = adler32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = adler32(adler, buffer, length);
     }
     if (adler != original_adler) error();
    */
    u32 adler32(u32 adler, const u8* buf, u32 len)
    {
#define DO1(buf,i)  {adler += (buf)[i]; sum2 += adler;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);
#define MOD(a) a %= BASE
#define MOD28(a) a %= BASE
#define MOD63(a) a %= BASE
        static const u32 BASE  = 65521U;
        static const u32 NMAX = 5552;

        // split Adler-32 into component sums
        u32 sum2 = (adler >> 16) & 0xFFFFU;
        adler &= 0xFFFFU;

        // in case user likes doing a byte at a time, keep it fast
        if(len == 1){
            adler += buf[0];
            if(adler >= BASE){
                adler -= BASE;
            }
            sum2 += adler;
            if(sum2 >= BASE){
                sum2 -= BASE;
            }
            return adler | (sum2 << 16);
        }

        // initial Adler-32 value (deferred check for len == 1 speed)
        if(CPPZ_NULL == buf){
            return 1UL;
        }

        // in case short lengths are provided, keep it somewhat fast
        if(len < 16){
            while(len--){
                adler += *buf++;
                sum2 += adler;
            }
            if(BASE <= adler){
                adler -= BASE;
            }
            MOD28(sum2); // only added so many BASE's
            return adler | (sum2 << 16);
        }

        // do length NMAX blocks -- requires just one modulo operation
        while(NMAX <= len){
            len -= NMAX;
            u32 n = NMAX / 16; // NMAX is divisible by 16
            do{
                DO16(buf); // 16 sums unrolled
                buf += 16;
            }while(--n);
            MOD(adler);
            MOD(sum2);
        }

        // do remaining bytes (less than NMAX, still just one modulo)
        if(len){ // avoid modulos if none remaining
            while(16<=len){
                len -= 16;
                DO16(buf);
                buf += 16;
            }
            while(len--){
                adler += *buf++;
                sum2 += adler;
            }
            MOD(adler);
            MOD(sum2);
        }

        // return recombined sums
        return adler | (sum2 << 16);
    }

    //Reverse the bytes in a 32-bit value
    inline u32 reverse(u32 v)
    {
        return ((v>>24)&0xFFU) | ((v>>8)&0xFF00U) | ((v&0xFF00U)<<8) | ((v&0xFFU)<<24);
    }

    //rank Z_BLOCK between Z_NO_FLUSH and Z_PARTIAL_FLUSH
    inline s32 rank(s32 f)
    {
        return (f * 2) - (f > 4 ? 9 : 0);
    }

    /**
    @brief Output a byte on the stream.
    @warn there is enough room in pending_buff_.
    */
    inline void put_byte(DeflateContext& context, u8 c)
    {
        context.pending_buff_[context.pending_++] = c;
    }

    /**
    @brief a short LSB first on the stream.
    @warn there is enough room in pending_buff_.
    */
    inline void put_short(DeflateContext& context, u16 c)
    {
        context.pending_buff_[context.pending_++] = (u8)(c&0xFFU);
        context.pending_buff_[context.pending_++] = (u8)((c>>8)&0xFFU);
    }

    /**
    @brief Put a short in the pending buffer. The 16-bit value is put in MSB order.
    @warn the stream state is correct and there is enough room in pending_buff_.
    */
    inline void putShortMSB(DeflateContext& context, u32 b)
    {
        put_byte(context, (u8)(b >> 8));
        put_byte(context, (u8)(b & 0xFFU));
    }

    void tree_tally_lit(DeflateContext& context, u8 c, s32& flush)
    {
        context.dbuff_[context.treeContext_.last_lit_] = 0;
        context.lbuff_[context.treeContext_.last_lit_++] = c;
        context.treeContext_.dyn_ltree_[c].Freq++;
        flush = (context.treeContext_.last_lit_ == (context.lit_bufsize_-1));
    }

    void tree_tally_dist(DeflateContext& context, u32 distance, u32 length, s32& flush)
    {
        u16 dist = static_cast<u16>(distance);
        u8 len = static_cast<u8>(length);
        context.dbuff_[context.treeContext_.last_lit_] = dist;
        context.lbuff_[context.treeContext_.last_lit_++] = len;
        --dist;
        context.treeContext_.dyn_ltree_[_length_code[len] + LITERALS + 1].Freq++;
        context.treeContext_.dyn_dtree_[d_code(dist)].Freq++;
        flush = (context.treeContext_.last_lit_ == (context.lit_bufsize_-1));
    }

    void send_bits(DeflateContext& context, s32 value, s32 length)
    {
        static const s32 BUF_SIZE = 16;
        TreeContext& tree = context.treeContext_;
        int len = length;
        if((BUF_SIZE - len) < tree.bi_valid_){
            tree.bi_buf_ |= (u16)value << tree.bi_valid_;
            put_short(context, tree.bi_buf_);
            tree.bi_buf_ = (u16)value >> (BUF_SIZE - tree.bi_valid_);
            tree.bi_valid_ += len - BUF_SIZE;
        } else {
            tree.bi_buf_ |= (u16)value << tree.bi_valid_;
            tree.bi_valid_ += len;
        }
    }

    /**
    @brief Send a code of the given tree. c and tree must not have side effects
    */
    inline void send_code(DeflateContext& context, s32 c, const ct_data* tree)
    {
        send_bits(context, tree[c].Code, tree[c].Len);
    }

    /**
    @brief Send a literal or distance tree in compressed form, using the codes in bl_tree.
    @param context
    @param tree ... the tree to be scanned
    @param max_code ... the tree's largest code of non zero frequency
    */
    void send_tree(DeflateContext& context, ct_data* tree, s32 max_code)
    {
        TreeContext& treeContext = context.treeContext_;
        s32 prevlen = -1;
        s32 nextlen = tree[0].Len;
        s32 count = 0;
        s32 max_count = 7;
        s32 min_count = 4;
        if(nextlen<=0){
            max_count = 138;
            min_count = 3;
        }
        for(s32 n=0; n<=max_code; ++n){
            CPPZ_ASSERT(0<=nextlen);
            s32 curlen = nextlen;
            nextlen = tree[n+1].Len;
            if(++count<max_count && curlen == nextlen){
                continue;
            }else if(count<min_count){
                do{ send_code(context, curlen, treeContext.bl_tree_);}while(--count != 0);
            }else if(0<curlen){
                if(curlen != prevlen){
                    send_code(context, curlen, treeContext.bl_tree_);
                    --count;
                }
                CPPZ_ASSERT(3<=count && count<=6);
                send_code(context, REP_3_6, treeContext.bl_tree_);
                send_bits(context, count-3, 2);
            }else if(count<=10){
                send_code(context, REPZ_3_10, treeContext.bl_tree_);
                send_bits(context, count-3, 3);
            }else{
                send_code(context, REPZ_11_138, treeContext.bl_tree_);
                send_bits(context, count-11, 7);
            }
            count = 0;
            prevlen = curlen;
            if(nextlen<=0){
                max_count = 138;
                min_count = 3;
            }else if(curlen == nextlen){
                max_count = 6;
                min_count = 3;
            }else{
                max_count = 7;
                min_count = 4;
            }
        } //for(s32 n=0
    }

    /**
    @brief Send the header for a block using dynamic Huffman trees: the counts, the lengths of the bit length codes, the literal tree and the distance tree.
    */
    void send_all_trees(DeflateContext& context, s32 lcodes, s32 dcodes, s32 blcodes)
    {
        CPPZ_ASSERT(257<=lcodes && 1<=dcodes && 4<=blcodes);
        CPPZ_ASSERT(lcodes<=L_CODES && dcodes<=D_CODES && blcodes<=BL_CODES);

        send_bits(context, lcodes-257, 5);
        send_bits(context, dcodes-1, 5);
        send_bits(context, blcodes-4, 4);
        for(s32 rank=0; rank<blcodes; ++rank){
            send_bits(context, context.treeContext_.bl_tree_[bl_order[rank]].Len, 3);
        }
        send_tree(context, context.treeContext_.dyn_ltree_, lcodes-1);
        send_tree(context, context.treeContext_.dyn_dtree_, dcodes-1);
    }

    void* cppz_alloc(size_t size, void* )
    {
        return CPPZ_MALLOC(size);
    }

    void cppz_free(void* ptr, void*)
    {
        CPPZ_FREE(ptr);
    }


    enum State
    {
        State_Init = 42,
        State_Busy = 113,
        State_Finish = 666,
    };

    enum BlockState
    {
        BlockState_NeedMore,
        BlockState_BlockDone,
        BlockState_FinishStarted,
        BlockState_FinishDone,
    };

    inline u32 max_dist(const DeflateContext& context)
    {
        return context.w_size_-MIN_LOOKAHEAD;
    }

    void clear_hash(DeflateContext& context)
    {
        context.head_[context.hash_size_-1] = 0;
        memset(context.head_, 0, sizeof(*context.head_)*(context.hash_size_-1));
    }

    void longestMatch_init(DeflateContext& context)
    {
        context.window_size_ = context.w_size_*2;
        clear_hash(context);

        context.good_match_ = 8;
        context.max_lazy_match_ = 16;
        context.nice_match_ = 128;
        context.max_chain_length_ = 128;

        context.strstart_ = 0;
        context.block_start_ = 0;
        context.lookahead_ = 0;
        context.insert_ = 0;
        context.match_length_ = MIN_MATCH-1;
        context.prev_length_ = MIN_MATCH-1;
        context.match_available_ = 0;
        context.ins_h_ = 0;
    }

    inline s32 deflateStateCheck(Stream& stream)
    {
        CPPZ_ASSERT(CPPZ_NULL != stream.alloc_func_);
        CPPZ_ASSERT(CPPZ_NULL != stream.free_func_);
        CPPZ_ASSERT(CPPZ_NULL != stream.deflate_);
        s32 state = stream.deflate_->state_;
        switch(state){
        case State_Init:
        case State_Busy:
        case State_Finish:
            return 0;
        default:
            return 1;
        }
    }

    inline s32 inflateStateCheck(Stream& stream)
    {
        CPPZ_ASSERT(CPPZ_NULL != stream.alloc_func_);
        CPPZ_ASSERT(CPPZ_NULL != stream.free_func_);
        CPPZ_ASSERT(CPPZ_NULL != stream.inflate_);

        InflateContext& context = *stream.inflate_;
        return (context.mode_<CPPZ_MODE_HEAD || CPPZ_MODE_SYNC<context.mode_)? 1 : 0;
    }

    void bi_flush(DeflateContext& context)
    {
        if(16 == context.treeContext_.bi_valid_){
            put_short(context, context.treeContext_.bi_buf_);
            context.treeContext_.bi_buf_ = 0;
            context.treeContext_.bi_valid_ = 0;
        }else if(8<=context.treeContext_.bi_valid_){
            put_byte(context, static_cast<u8>(context.treeContext_.bi_buf_));
            context.treeContext_.bi_buf_ >>= 8;
            context.treeContext_.bi_valid_ -= 8;
        }
    }

    void flush_bits(DeflateContext& context)
    {
        bi_flush(context);
    }


    /**
    Flush the bit buffer and align the output on a byte boundary
    */
    void bi_windup(DeflateContext& context)
    {
        if(8<context.treeContext_.bi_valid_){
            put_short(context, context.treeContext_.bi_buf_);
        }else if(0<context.treeContext_.bi_valid_){
            put_byte(context, static_cast<u8>(context.treeContext_.bi_buf_));
        }
        context.treeContext_.bi_buf_ = 0;
        context.treeContext_.bi_valid_ = 0;
    }

    /**
    Send one empty static block to give enough lookahead for inflate.
    This takes 10 bits, of which 7 may remain in the bit buffer.
    */
    void tree_align(DeflateContext& context)
    {
        send_bits(context, CPPZ_STATIC_TREES<<1, 3);
        send_code(context, END_BLOCK, static_ltree);
        bi_flush(context);
    }


    /**
    @brief Send a stored block
    @param context ... 
    @param buffer ... input block
    @param stored_len ... length of input block
    @param last ... one if this is the last block for a file
    */
    void tree_stored_block(DeflateContext& context, u8* buffer, u32 stored_len, s32 last)
    {
        send_bits(context, (CPPZ_STORED_BLOCK<<1)+last, 3); //send block type
        bi_windup(context); //align on byte boundary
        put_short(context, static_cast<u16>(stored_len));
        put_short(context, static_cast<u16>(~stored_len));
        memcpy(context.pending_buff_+context.pending_, buffer, stored_len);
        context.pending_ += stored_len;
    }

    /**
    @brief Send the block data compressed using the given Huffman trees
    @param context
    @param ltree ... literal tree
    @param dtree ... distance tree
    */
    void compress_block(DeflateContext& context, const ct_data* ltree, const ct_data* dtree)
    {
        TreeContext& treeContext = context.treeContext_;

        if(treeContext.last_lit_<=0){
            send_code(context, END_BLOCK, ltree);
            return;
        }
        u32 lx = 0; //running index in l_buf
        do{
            u32 dist = context.dbuff_[lx]; //distance of matched string
            s32 lc = context.lbuff_[lx]; //match length or unmatched char
            ++lx;
            if(dist<=0){
                send_code(context, lc, ltree); //send a literal byte
            }else{
                //
                u32 code = _length_code[lc];
                send_code(context, code+LITERALS+1, ltree); //send the length code
                s32 extra = extra_lbits[code];
                if(0<extra){
                    lc -= base_length[code];
                    send_bits(context, lc, extra); //send the extra length bits
                }
                --dist; //distance is now the match distance - 1
                code = d_code(static_cast<u16>(dist));
                send_code(context , code, dtree); //send the distance code;
                extra = extra_dbits[code];
                if(0<extra){
                    dist -= base_dist[code];
                    send_bits(context, dist, extra); //send the extra distance bits
                }
            } //literal or match pair ?
        }while(lx < treeContext.last_lit_);

        send_code(context, END_BLOCK, ltree);
    }
    

    /**
    @param context ... 
    @param buffer ... input block, or NULL if too old
    @param stored_len ... length of input block
    @param last ... one if this is the last block for a file

    Determine the best encoding for the current block: dynamic trees, static trees or store, and write out the encoded block.
    */
    void tree_flush_block(DeflateContext& context, u8* buffer, u32 stored_len, s32 last)
    {
        TreeContext& treeContext = context.treeContext_;
        s32 max_blindex = 0;
        u32 opt_lenb, static_lenb; //opt_len and static_len in bytes

        //Build the Huffman trees unless a stored block is forced
        if(0<context.level_){
            //Construct the literal and distance trees
            build_tree(treeContext, treeContext.literal_tree_);
            build_tree(treeContext, treeContext.distance_tree_);

            // At this point, opt_len and static_len are the total bit lengths of the compressed block data, excluding the tree representations.

        // Build the bit length tree for the above two trees, and get the index in bl_order of the last bit length code to send.
            max_blindex = build_bl_tree(treeContext);

            //Determine the best encoding. Compute the block lengths in bytes
            opt_lenb = (treeContext.opt_len_+3+7)>>3;
            static_lenb = (treeContext.static_len_+3+7)>>3;
            if(static_lenb<=opt_lenb){
                opt_lenb = static_lenb;
            }
        } else{
            opt_lenb = static_lenb = stored_len+5; //force a stored block
        }
        if((stored_len+4) <= opt_lenb && CPPZ_NULL != buffer){
            /* The test buf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
            Otherwise we can't have processed more than WSIZE input bytes since
            the last block flush, because compression would have been
            successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
            transform a block into a stored block.*/
            tree_stored_block(context, buffer, stored_len, last);
        } else if(CPPZ_FIXED == context.strategy_ || static_lenb == opt_lenb){
            send_bits(context, (CPPZ_STATIC_TREES<<1)+last, 3);
            compress_block(context, static_ltree, static_dtree);
        } else{
            send_bits(context, (CPPZ_DYN_TREES<<1)+last, 3);
            send_all_trees(context, treeContext.literal_tree_.maxCode_+1, treeContext.distance_tree_.maxCode_+1, max_blindex+1);
            compress_block(context, treeContext.dyn_ltree_, treeContext.dyn_dtree_);
        }

        // The above check is made mod 2^32, for files larger than 512 MB and uLong implemented on 32 bits.
        init_block(treeContext);
        if(last){
            bi_windup(context);
        }
    }

    /**
    Flush as much pending output as possible. All deflate() output, except for
    some deflate_stored() output, goes through this function so some
    applications may wish to modify it to avoid allocating a large
    strm->next_out buffer and copying into it. (See also read_buf()).
    */
    void flush_pending(Stream& stream)
    {
        DeflateContext& context = *stream.deflate_;

        flush_bits(context);
        u32 len = context.pending_;
        if(stream.avail_out_<len){
            len = stream.avail_out_;
        }
        if(len<=0){
            return;
        }
        memcpy(stream.next_out_, context.pendingOut_, len);
        stream.next_out_ += len;
        context.pendingOut_ += len;
        stream.total_out_ += len;
        stream.avail_out_ -= len;
        context.pending_ -= len;
        if(context.pending_<=0){
            context.pendingOut_ = context.pending_buff_;
        }
    }

    /**
    Flush the current block, with given end-of-file flag.
    IN assertion: strstart is set to the end of the current match.
    */
    void flush_block_only(Stream& stream, s32 last)
    {
        DeflateContext& context = *stream.deflate_;
        tree_flush_block(context,
            (0<=context.block_start_? (u8*)&context.window_[context.block_start_] : CPPZ_NULL),
            (s32)context.strstart_ - context.block_start_,
            last);
        context.block_start_ = context.strstart_;
        flush_pending(stream);
    }

    // Same but force premature exit if necessary.
#define FLUSH_BLOCK(stream, last) { \
   flush_block_only(stream, last); \
   if (stream.avail_out_ == 0) return (last) ? BlockState_FinishStarted : BlockState_NeedMore; \
}

    /**
    @brief Update a hash value with the given input byte
    */
    void update_hash(DeflateContext& context, u32& hash, u32 c)
    {
        hash = ((hash<<context.hash_shift_) ^ c) & context.hash_mask_;
    }

    /**
    Slide the hash table when sliding the window down (could be avoided with 32 bit values at the expense of memory usage).
    We slide even when level == 0 to keep the hash table consistent if we switch back to level > 0 later.
    */
    void slide_hash(DeflateContext& context)
    {
        u32 wsize = context.w_size_;
        u32 n = context.hash_size_;
        u16* p = &context.head_[n];

        do{
            u32 m = *(--p);
            *p = static_cast<u16>((wsize<=m)? m-wsize : 0);
        }while(--n);
        n = wsize;
        p = &context.prev_[n];
        do{
            u32 m = *(--p);
            *p = static_cast<u16>((wsize<=m)? m-wsize : 0);
        }while(--n);
    }


    /**
    Insert string str in the dictionary and set match_head to the previous head
    of the hash chain (the most recent string with same hash key). Return
    the previous length of the hash chain.
    If this file is compiled with -DFASTEST, the compression level is forced
    to 1, and no hash chains are maintained.
    IN  assertion: all calls to INSERT_STRING are made with consecutive input
       characters and the first MIN_MATCH bytes of str are valid (except for
       the last MIN_MATCH-1 bytes of the input file).
    */
    void insert_string(DeflateContext& context, u32 str, u32& match_head)
    {
        update_hash(context, context.ins_h_, context.window_[str+(MIN_MATCH-1)]);
        match_head = context.prev_[str & context.w_mask_] = context.head_[context.ins_h_];
        context.head_[context.ins_h_] = static_cast<u16>(str);
    }

    template<bool enable>
    inline void limit_size(DeflateContext&, u32&, u32)
    {
    }
    template<>
    inline void limit_size<true>(DeflateContext& context, u32& more, u32 wsize)
    {
        if(more<=0 && context.strstart_<=0 && context.lookahead_<=0){
            more = wsize;
        }else if(more == (u32)(-1)){
            --more;
        }
    }

    /**
    Read a new buffer from the current input stream, update the adler32 and total number of bytes read.
    All deflate() input goes through this function so some applications may wish to modify it to avoid allocating a large strm->next_in buffer and copying from it.
    */
    u32 read_buf(Stream& stream, u8* buffer, u32 size)
    {
        u32 len = stream.avail_in_;
        if(size<len){
            len = size;
        }
        if(len<=0){
            return 0;
        }
        stream.avail_in_ -= len;
        memcpy(buffer, stream.next_in_, len);
        if(1 == stream.deflate_->wrap_){
            stream.adler32_ = adler32(stream.adler32_, buffer, len);
        }
        stream.next_in_ += len;
        stream.total_in_ += len;
        return len;
    }

    /**
    @brief Fill the window when the lookahead becomes insufficient. Updates strstart and lookahead.
    */
    void fill_window(Stream& stream)
    {
        DeflateContext& context = *stream.deflate_;
        u32 wsize = stream.deflate_->w_size_;
        do{
            u32 more = (context.window_size_ - context.lookahead_ - context.strstart_);
            limit_size<sizeof(s32)<=2>(context, more, wsize);

            /* If the window is almost full and there is insufficient lookahead,
            * move the upper half to the lower one to make room in the upper half.
            */
            if((wsize+max_dist(context))<=context.strstart_){
                memcpy(context.window_, context.window_+wsize, wsize-more);
                context.match_start_ -= wsize;
                context.strstart_ -= wsize;
                context.block_start_ -= wsize;
                slide_hash(context);
                more += wsize;
            }
            if(stream.avail_in_<=0){
                break;
            }
            CPPZ_ASSERT(2<=more);
            u32 n = read_buf(stream, context.window_+context.strstart_+context.lookahead_, more);
            context.lookahead_ += n;

            //Initialize the hash value now that we have some input
            if(MIN_MATCH<=(context.lookahead_+context.insert_)){
                u32 str = context.strstart_ - context.insert_;
                context.ins_h_ = context.window_[str];
                update_hash(context, context.ins_h_, context.window_[str+1]);
                
                while(context.insert_){
                    update_hash(context, context.ins_h_, context.window_[str+MIN_MATCH-1]);
                    context.prev_[str&context.w_mask_] = context.head_[context.ins_h_];
                    context.head_[context.ins_h_] = static_cast<u16>(str);
                    ++str;
                    --context.insert_;
                    if((context.lookahead_+context.insert_)<MIN_MATCH){
                        break;
                    }
                }
            }//if(MIN_MATCH
        }while(context.lookahead_<MIN_LOOKAHEAD && 0<stream.avail_in_);

        /* If the WIN_INIT bytes after the end of the current data have never been
        written, then zero those bytes in order to avoid memory check reports of
        the use of uninitialized (or uninitialised as Julian writes) bytes by
        the longest match routines.  Update the high water mark for the next
        time through here.  WIN_INIT is set to MAX_MATCH since the longest match
        routines allow scanning to strstart + MAX_MATCH, ignoring lookahead.
        */
        if(context.high_water_<context.window_size_){
            u32 curr = context.strstart_ + context.lookahead_;
            if(context.high_water_<curr){
                // Previous high water mark below current data -- zero WIN_INIT bytes or up to end of window, whichever is less.
                u32 init = context.window_size_ - curr;
                if(WIN_INIT<init){
                    init = WIN_INIT;
                }
                memset(context.window_+curr, 0, init);
                context.high_water_ = curr + init;
            }else if(context.high_water_<curr + WIN_INIT){
                // High water mark at or above current data, but below current data plus WIN_INIT -- zero out to current data plus WIN_INIT, or up to end of window, whichever is less.
                u32 init = curr + WIN_INIT - context.high_water_;
                u32 size = context.window_size_ - context.high_water_;
                if(size<init){
                    init = size;
                }
                memset(context.window_+context.high_water_, 0, init);
                context.high_water_ += init;
            }
        }//if(context.high_water_
        CPPZ_ASSERT(context.strstart_<=(context.window_size_-MIN_LOOKAHEAD));
    }

    /**
    Set match_start to the longest match starting at the given string and return its length. Matches shorter or equal to prev_length are discarded,
    in which case the result is equal to prev_length and match_start is garbage
    */
    u32 longest_match(DeflateContext& context, u32 cur_match)
    {
        u32 chain_length = context.max_chain_length_; //max hash chain length
        u8* scan = context.window_ + context.strstart_; //current string
        s32 best_len = context.prev_length_;
        u32 nice_match = context.nice_match_;
        u32 limit = (max_dist(context)<context.strstart_)? context.strstart_-max_dist(context) : 0;
        // Stop when cur_match becomes <= limit. To simplify the code, we prevent matches with the string of window index 0.
        u8* strend = context.window_ + context.strstart_ + MAX_MATCH;
        u8 scan_end1 = scan[best_len-1];
        u8 scan_end = scan[best_len];
        CPPZ_ASSERT(8<=context.hash_bits_ && 258==MAX_MATCH);

        //Do not waste too much time if we already have a good match
        if(context.good_match_<=context.prev_length_){
            chain_length >>= 2;
        }
        //Do not look for matches beyond the end of the input. This is necessary to make deflate deterministic.
        if(context.lookahead_<nice_match){
            nice_match = context.lookahead_;
        }
        CPPZ_ASSERT(context.strstart_<=(context.window_size_-MIN_LOOKAHEAD));
        do{
            CPPZ_ASSERT(cur_match<context.strstart_);
            u8* match = context.window_ + cur_match;

            if(match[best_len] != scan_end
                || match[best_len-1] != scan_end1
                || *match != *scan
                || *++match != scan[1])
            {
                continue;
            }
            scan += 2;
            ++match;
            CPPZ_ASSERT(*scan == *match);
            // We check for insufficient lookahead only every 8th comparison. the 256th check will be made at strstart+258.
            do{
            }while(*++scan == *++match && *++scan == *++match
                && *++scan == *++match && *++scan == *++match
                && *++scan == *++match && *++scan == *++match
                && *++scan == *++match && *++scan == *++match
                && scan<strend);
            CPPZ_ASSERT(scan<=context.window_+(context.window_size_-1));
            s32 len = MAX_MATCH - (s32)(strend-scan);
            scan = strend - MAX_MATCH;
            if(best_len < len){
                context.match_start_ = cur_match;
                best_len = len;
                if(nice_match<=static_cast<u32>(len)){
                    break;
                }
                scan_end1 = scan[best_len-1];
                scan_end = scan[best_len];
            }
        }while(limit<(cur_match=context.prev_[cur_match & context.w_mask_]) && --chain_length!=0);
        if(static_cast<u32>(best_len)<=context.lookahead_){
            return best_len;
        }
        return context.lookahead_;
    }

#define check_match(s, start, match, length)

    /**
    Same as above, but achieves better compression. We use a lazy
    evaluation for matches: a match is finally adopted only if there is
    no better match at the next window position.
    */
    BlockState deflate_slow(Stream& stream, s32 flush)
    {
        DeflateContext& context = *stream.deflate_;

        static const s32 TOO_FAR = 4096;
        s32 bflush; //set if current block must be flushed

        //Process the input block.
        for(;;){
            /* Make sure that we always have enough lookahead, except
            * at the end of the input file. We need MAX_MATCH bytes
            * for the next match, plus MIN_MATCH bytes to insert the
            * string following the next match.
            */
            if(context.lookahead_<MIN_LOOKAHEAD){
                fill_window(stream);
                if(context.lookahead_<MIN_LOOKAHEAD && CPPZ_NO_FLUSH==flush){
                    return BlockState_NeedMore;
                }
                if(context.lookahead_<=0){ //flush the current block
                    break;
                }
            }
            // Insert the string window[strstart .. strstart+2] in the dictionary, and set hash_head to the head of the hash chain:
            u32 hash_head = 0; //head of hash chain
            if(MIN_MATCH<=context.lookahead_){
                insert_string(context, context.strstart_, hash_head);
            }

            // Find the longest match, discarding those <= prev_length.
            context.prev_length_ = context.match_length_;
            context.prev_match_ = context.match_start_;
            context.match_length_ = MIN_MATCH - 1;

            if(0 != hash_head
                && context.prev_length_<context.max_lazy_match_
                && (context.strstart_ - hash_head) <= max_dist(context))
            {
                /* To simplify the code, we prevent matches with the string
                * of window index 0 (in particular we have to avoid a match
                * of the string with itself at the start of the input file).
                */
                context.match_length_ = longest_match(context, hash_head); //longest_match() sets match_start
                if(context.match_length_<=5
                    && (CPPZ_FILTERED == context.strategy_
                        || (MIN_MATCH == context.match_length_ && TOO_FAR<(context.strstart_-context.match_start_))))
                {
                    /* If prev_match is also MIN_MATCH, match_start is garbage
                    * but we will ignore the current match anyway.
                    */
                    context.match_length_ = MIN_MATCH-1;
                }

            }//if(0 != hash_head

            /* If there was a match at the previous step and the current
            * match is not better, output the previous match:
            */
            if(MIN_MATCH<=context.prev_length_
                && context.match_length_<=context.prev_length_)
            {
                u32 max_insert = context.strstart_ + context.lookahead_ - MIN_MATCH;
                check_match(context, context.strstart_-1, context.prev_match_, context.prev_length_);
                tree_tally_dist(context, context.strstart_-1-context.prev_match_, context.prev_length_-MIN_MATCH, bflush);

                /* Insert in hash table all strings up to the end of the match.
                * strstart-1 and strstart are already inserted. If there is not
                * enough lookahead, the last two strings are not inserted in
                * the hash table.
                */
                context.lookahead_ -= context.prev_length_-1;
                context.prev_length_ -= 2;
                do{
                    if(++context.strstart_<=max_insert){
                        insert_string(context, context.strstart_, hash_head);
                    }
                }while(--context.prev_length_ != 0);
                context.match_available_ = 0;
                context.match_length_ = MIN_MATCH-1;
                ++context.strstart_;
                if(bflush){
                    FLUSH_BLOCK(stream, 0);
                }
            }else if(context.match_available_){ //if(MIN_MATCH<=context.prev_length_
                /* If there was no match at the previous position, output a
                * single literal. If there was a match but the current match
                * is longer, truncate the previous match to a single literal.
                */
                tree_tally_lit(context, context.window_[context.strstart_-1], bflush);
                if(bflush){
                    flush_block_only(stream, 0);
                }
                ++context.strstart_;
                --context.lookahead_;
                if(0==stream.avail_out_){
                    return BlockState_NeedMore;
                }
            }else{ //}else if(context.match_available_
                context.match_available_ = 1;
                ++context.strstart_;
                --context.lookahead_;
            } //if(MIN_MATCH<=context.prev_length_
        }//for(;;)

        CPPZ_ASSERT(CPPZ_NO_FLUSH != flush);
        if(context.match_available_){
            tree_tally_lit(context, context.window_[context.strstart_-1], bflush);
            context.match_available_ = 0;
        }
        context.insert_ = context.strstart_ < (MIN_MATCH-1)? context.strstart_ : (MIN_MATCH-1);
        if(flush == CPPZ_FINISH){
            FLUSH_BLOCK(stream, 1);
            return BlockState_FinishDone;
        }
        if(context.treeContext_.last_lit_){
            FLUSH_BLOCK(stream, 0);
        }
        return BlockState_BlockDone;
    }

    //Load registers with state in inflate() for speed
    inline void load(const u8*& next, u8*& put, u32& left, u32& have, u32& hold, u32& bits, Stream& stream)
    {
        next = stream.next_in_;
        put = stream.next_out_;
        left = stream.avail_out_;
        have = stream.avail_in_;
        hold = stream.inflate_->hold_;
        bits = stream.inflate_->bits_;
    }

    //Get a byte of input into the bit accumulator, or return from inflate() if there is no input available.
    inline bool pullbyte(const u8*& next, u32& have, u32& hold, u32& bits)
    {
        if(have<=0){
            return false;
        }
        --have;
        hold += (*next++)<<bits;
        bits += 8;
        return true;
    }
#define PULLBYTE(next, have, hold, bits) if(!pullbyte(next, have, hold, bits)) goto INF_LEAVE

    inline void restore(Stream& stream, u8* put, u32 left, const u8* next, u32 have, u32 hold, u32 bits)
    {
        stream.next_out_ = put;
        stream.avail_out_ = left;
        stream.next_in_ = next;
        stream.avail_in_ = have;
        stream.inflate_->hold_ = hold;
        stream.inflate_->bits_ = bits;
    }

    //Clear the input bit accumulator
    inline void initbits(u32& hold, u32& bits)
    {
        hold = 0;
        bits = 0;
    }

    //Assure that there are at least n bits in the bit accumulator.  If there is not enough available input to do that, then return from inflate().
    inline bool needbits(const u8*& next, u32& have, u32& hold, u32& bits, u32 n)
    {
        while(bits<n){
            if(!pullbyte(next, have, hold, bits)){
                return false;
            }
        }
        return true;
    }
#define NEEDBITS(next, have, hold, bits, n) if(!needbits(next, have, hold, bits, n)) goto INF_LEAVE

    //Return the low n bits of the bit accumulator (n < 16)
    inline u32 get_bits(u32 hold, u32 n)
    {
        return hold & ((0x1U<<n)-1);
    }

    //Remove n bits from the bit accumulator
    inline void dropbits(u32& hold, u32& bits, u32 n)
    {
        hold >>= n;
        bits -= n;
    }

    //Remove zero to seven bits as needed to go to a byte boundary
    inline void bytebits(u32& hold, u32& bits)
    {
        hold >>= (bits & 7);
        bits -= (bits & 7);
    }

    s32 updatewindow(Stream& stream, const u8* end, u32 copy)
    {
        InflateContext& context = *stream.inflate_;
        CPPZ_ASSERT(context.wbits_<=15);
        if(context.wsize_<=0){
            context.wsize_ = 1U<<context.wbits_;
            context.wnext_ = 0;
            context.whave_ = 0;
        }
        if(context.wsize_<=copy){
            memcpy(context.window_, end-context.wsize_, context.wsize_);
            context.wnext_ = 0;
            context.whave_ = context.wsize_;
        }else{
            u32 dist = context.wsize_ - context.wnext_;
            if(copy<dist){
                dist = copy;
            }
            memcpy(context.window_+context.wnext_, end-copy, dist);
            copy -= dist;
            if(copy){
                memcpy(context.window_, end-copy, copy);
                context.wnext_ = copy;
                context.whave_ = context.wsize_;
            }else{
                context.wnext_ += dist;
                if(context.wnext_ == context.wsize_){
                    context.wnext_ = 0;
                }
                if(context.whave_<context.wsize_){
                    context.whave_ += dist;
                }
            }
        }//if(context.wsize_
        return 0;
    }

    static const CODE lenfix[512] =
    {
        {96,7,0},{0,8,80},{0,8,16},{20,8,115},{18,7,31},{0,8,112},{0,8,48},
        {0,9,192},{16,7,10},{0,8,96},{0,8,32},{0,9,160},{0,8,0},{0,8,128},
        {0,8,64},{0,9,224},{16,7,6},{0,8,88},{0,8,24},{0,9,144},{19,7,59},
        {0,8,120},{0,8,56},{0,9,208},{17,7,17},{0,8,104},{0,8,40},{0,9,176},
        {0,8,8},{0,8,136},{0,8,72},{0,9,240},{16,7,4},{0,8,84},{0,8,20},
        {21,8,227},{19,7,43},{0,8,116},{0,8,52},{0,9,200},{17,7,13},{0,8,100},
        {0,8,36},{0,9,168},{0,8,4},{0,8,132},{0,8,68},{0,9,232},{16,7,8},
        {0,8,92},{0,8,28},{0,9,152},{20,7,83},{0,8,124},{0,8,60},{0,9,216},
        {18,7,23},{0,8,108},{0,8,44},{0,9,184},{0,8,12},{0,8,140},{0,8,76},
        {0,9,248},{16,7,3},{0,8,82},{0,8,18},{21,8,163},{19,7,35},{0,8,114},
        {0,8,50},{0,9,196},{17,7,11},{0,8,98},{0,8,34},{0,9,164},{0,8,2},
        {0,8,130},{0,8,66},{0,9,228},{16,7,7},{0,8,90},{0,8,26},{0,9,148},
        {20,7,67},{0,8,122},{0,8,58},{0,9,212},{18,7,19},{0,8,106},{0,8,42},
        {0,9,180},{0,8,10},{0,8,138},{0,8,74},{0,9,244},{16,7,5},{0,8,86},
        {0,8,22},{64,8,0},{19,7,51},{0,8,118},{0,8,54},{0,9,204},{17,7,15},
        {0,8,102},{0,8,38},{0,9,172},{0,8,6},{0,8,134},{0,8,70},{0,9,236},
        {16,7,9},{0,8,94},{0,8,30},{0,9,156},{20,7,99},{0,8,126},{0,8,62},
        {0,9,220},{18,7,27},{0,8,110},{0,8,46},{0,9,188},{0,8,14},{0,8,142},
        {0,8,78},{0,9,252},{96,7,0},{0,8,81},{0,8,17},{21,8,131},{18,7,31},
        {0,8,113},{0,8,49},{0,9,194},{16,7,10},{0,8,97},{0,8,33},{0,9,162},
        {0,8,1},{0,8,129},{0,8,65},{0,9,226},{16,7,6},{0,8,89},{0,8,25},
        {0,9,146},{19,7,59},{0,8,121},{0,8,57},{0,9,210},{17,7,17},{0,8,105},
        {0,8,41},{0,9,178},{0,8,9},{0,8,137},{0,8,73},{0,9,242},{16,7,4},
        {0,8,85},{0,8,21},{16,8,258},{19,7,43},{0,8,117},{0,8,53},{0,9,202},
        {17,7,13},{0,8,101},{0,8,37},{0,9,170},{0,8,5},{0,8,133},{0,8,69},
        {0,9,234},{16,7,8},{0,8,93},{0,8,29},{0,9,154},{20,7,83},{0,8,125},
        {0,8,61},{0,9,218},{18,7,23},{0,8,109},{0,8,45},{0,9,186},{0,8,13},
        {0,8,141},{0,8,77},{0,9,250},{16,7,3},{0,8,83},{0,8,19},{21,8,195},
        {19,7,35},{0,8,115},{0,8,51},{0,9,198},{17,7,11},{0,8,99},{0,8,35},
        {0,9,166},{0,8,3},{0,8,131},{0,8,67},{0,9,230},{16,7,7},{0,8,91},
        {0,8,27},{0,9,150},{20,7,67},{0,8,123},{0,8,59},{0,9,214},{18,7,19},
        {0,8,107},{0,8,43},{0,9,182},{0,8,11},{0,8,139},{0,8,75},{0,9,246},
        {16,7,5},{0,8,87},{0,8,23},{64,8,0},{19,7,51},{0,8,119},{0,8,55},
        {0,9,206},{17,7,15},{0,8,103},{0,8,39},{0,9,174},{0,8,7},{0,8,135},
        {0,8,71},{0,9,238},{16,7,9},{0,8,95},{0,8,31},{0,9,158},{20,7,99},
        {0,8,127},{0,8,63},{0,9,222},{18,7,27},{0,8,111},{0,8,47},{0,9,190},
        {0,8,15},{0,8,143},{0,8,79},{0,9,254},{96,7,0},{0,8,80},{0,8,16},
        {20,8,115},{18,7,31},{0,8,112},{0,8,48},{0,9,193},{16,7,10},{0,8,96},
        {0,8,32},{0,9,161},{0,8,0},{0,8,128},{0,8,64},{0,9,225},{16,7,6},
        {0,8,88},{0,8,24},{0,9,145},{19,7,59},{0,8,120},{0,8,56},{0,9,209},
        {17,7,17},{0,8,104},{0,8,40},{0,9,177},{0,8,8},{0,8,136},{0,8,72},
        {0,9,241},{16,7,4},{0,8,84},{0,8,20},{21,8,227},{19,7,43},{0,8,116},
        {0,8,52},{0,9,201},{17,7,13},{0,8,100},{0,8,36},{0,9,169},{0,8,4},
        {0,8,132},{0,8,68},{0,9,233},{16,7,8},{0,8,92},{0,8,28},{0,9,153},
        {20,7,83},{0,8,124},{0,8,60},{0,9,217},{18,7,23},{0,8,108},{0,8,44},
        {0,9,185},{0,8,12},{0,8,140},{0,8,76},{0,9,249},{16,7,3},{0,8,82},
        {0,8,18},{21,8,163},{19,7,35},{0,8,114},{0,8,50},{0,9,197},{17,7,11},
        {0,8,98},{0,8,34},{0,9,165},{0,8,2},{0,8,130},{0,8,66},{0,9,229},
        {16,7,7},{0,8,90},{0,8,26},{0,9,149},{20,7,67},{0,8,122},{0,8,58},
        {0,9,213},{18,7,19},{0,8,106},{0,8,42},{0,9,181},{0,8,10},{0,8,138},
        {0,8,74},{0,9,245},{16,7,5},{0,8,86},{0,8,22},{64,8,0},{19,7,51},
        {0,8,118},{0,8,54},{0,9,205},{17,7,15},{0,8,102},{0,8,38},{0,9,173},
        {0,8,6},{0,8,134},{0,8,70},{0,9,237},{16,7,9},{0,8,94},{0,8,30},
        {0,9,157},{20,7,99},{0,8,126},{0,8,62},{0,9,221},{18,7,27},{0,8,110},
        {0,8,46},{0,9,189},{0,8,14},{0,8,142},{0,8,78},{0,9,253},{96,7,0},
        {0,8,81},{0,8,17},{21,8,131},{18,7,31},{0,8,113},{0,8,49},{0,9,195},
        {16,7,10},{0,8,97},{0,8,33},{0,9,163},{0,8,1},{0,8,129},{0,8,65},
        {0,9,227},{16,7,6},{0,8,89},{0,8,25},{0,9,147},{19,7,59},{0,8,121},
        {0,8,57},{0,9,211},{17,7,17},{0,8,105},{0,8,41},{0,9,179},{0,8,9},
        {0,8,137},{0,8,73},{0,9,243},{16,7,4},{0,8,85},{0,8,21},{16,8,258},
        {19,7,43},{0,8,117},{0,8,53},{0,9,203},{17,7,13},{0,8,101},{0,8,37},
        {0,9,171},{0,8,5},{0,8,133},{0,8,69},{0,9,235},{16,7,8},{0,8,93},
        {0,8,29},{0,9,155},{20,7,83},{0,8,125},{0,8,61},{0,9,219},{18,7,23},
        {0,8,109},{0,8,45},{0,9,187},{0,8,13},{0,8,141},{0,8,77},{0,9,251},
        {16,7,3},{0,8,83},{0,8,19},{21,8,195},{19,7,35},{0,8,115},{0,8,51},
        {0,9,199},{17,7,11},{0,8,99},{0,8,35},{0,9,167},{0,8,3},{0,8,131},
        {0,8,67},{0,9,231},{16,7,7},{0,8,91},{0,8,27},{0,9,151},{20,7,67},
        {0,8,123},{0,8,59},{0,9,215},{18,7,19},{0,8,107},{0,8,43},{0,9,183},
        {0,8,11},{0,8,139},{0,8,75},{0,9,247},{16,7,5},{0,8,87},{0,8,23},
        {64,8,0},{19,7,51},{0,8,119},{0,8,55},{0,9,207},{17,7,15},{0,8,103},
        {0,8,39},{0,9,175},{0,8,7},{0,8,135},{0,8,71},{0,9,239},{16,7,9},
        {0,8,95},{0,8,31},{0,9,159},{20,7,99},{0,8,127},{0,8,63},{0,9,223},
        {18,7,27},{0,8,111},{0,8,47},{0,9,191},{0,8,15},{0,8,143},{0,8,79},
        {0,9,255}
    };

    static const CODE distfix[32] =
    {
        {16,5,1},{23,5,257},{19,5,17},{27,5,4097},{17,5,5},{25,5,1025},
        {21,5,65},{29,5,16385},{16,5,3},{24,5,513},{20,5,33},{28,5,8193},
        {18,5,9},{26,5,2049},{22,5,129},{64,5,0},{16,5,2},{23,5,385},
        {19,5,25},{27,5,6145},{17,5,7},{25,5,1537},{21,5,97},{29,5,24577},
        {16,5,4},{24,5,769},{20,5,49},{28,5,12289},{18,5,13},{26,5,3073},
        {22,5,193},{64,5,0}
    };
    void fixedtables(InflateContext& context)
    {
        context.lencode_ = lenfix;
        context.lenbits_ = 9;
        context.distcode_ = distfix;
        context.distbits_ = 5;
    }

    enum CodeType
    {
        CODES,
        LENS,
        DISTS,
    };

    s32 inflate_table(CodeType type, u16* lens, u32 codes, CODE** table, u32* bits, u16* work)
    {
        static const unsigned short lbase[31] = { // Length codes 257..285 base
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
        static const unsigned short lext[31] = { // Length codes 257..285 extra
            16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18,
            19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 77, 202};
        static const unsigned short dbase[32] = { // Distance codes 0..29 base
            1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
            257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
            8193, 12289, 16385, 24577, 0, 0};
        static const unsigned short dext[32] = { // Distance codes 0..29 extra
            16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
            23, 23, 24, 24, 25, 25, 26, 26, 27, 27,
            28, 28, 29, 29, 64, 64};

        u16 count[MAX_BITS+1];
        u16 offs[MAX_BITS+1];
        for(u32 len=0; len<=MAX_BITS; ++len){
            count[len] = 0;
        }
        for(u32 sym=0; sym<codes; ++sym){
            ++count[lens[sym]];
        }
        u32 root = *bits;
        u32 max;
        for(max = MAX_BITS; 1<=max; --max){
            if(0 < count[max]){
                break;
            }
        }
        if(max<root){
            root = max;
        }
        CODE here;
        if(max<=0){
            here.op_ = 64;
            here.bits_ = 1;
            here.val_ = 0;
            *(*table)++ = here;
            *(*table)++ = here;
            *bits = 1;
            return 0;
        }
        u32 min;
        for(min=1; min<max; ++min){
            if(0<count[min]){
                break;
            }
        }
        if(root<min){
            root = min;
        }
        s32 left=1;
        for(u32 len=1; len<=MAX_BITS; ++len){
            left <<= 1;
            left -= count[len];
            if(left<0){
                return -1;
            }
        }
        if(0<left && (type == CODES || 1 != max)){
            return -1;
        }
        offs[1] = 0;
        for(u32 len=1; len<MAX_BITS; ++len){
            offs[len+1] = offs[len] + count[len];
        }
        for(u32 sym=0; sym<codes; ++sym){
            if(0<lens[sym]){
                work[offs[lens[sym]]++] = static_cast<u16>(sym);
            }
        }

        const u16* base;
        const u16* extra;
        u32 match;
        switch(type)
        {
        case CODES:
            base = extra = work;
            match = 20;
            break;
        case LENS:
            base = lbase;
            extra = lext;
            match = 257;
            break;
        default:
            base = dbase;
            extra = dext;
            match = 0;
            break;
        }
        u32 used = 1U << root;
        if((LENS==type && ENOUGH_LENS<used)
            || (DISTS==type && ENOUGH_DISTS<used)){
            return 1;
        }
        u32 huff=0;
        u32 sym=0;
        u32 len = min;
        CODE* next = *table;
        u32 curr = root;
        u32 low = static_cast<u32>(-1);
        u32 mask = used-1;
        u32 drop = 0;
        for(;;){
            here.bits_ = static_cast<u8>(len-drop);
            if(work[sym]+1U < match){
                here.op_ = 0;
                here.val_ = work[sym];
            }else if(match<=work[sym]){
                here.op_ = static_cast<u8>(extra[work[sym] - match]);
                here.val_ = base[work[sym]-match];
            }else{
                here.op_ = (32+64);
                here.val_ = 0;
            }
            u32 incr = 1U << (len-drop);
            u32 fill = 1U << curr;
            min = fill;
            do{
                fill -= incr;
                next[(huff>>drop)+fill] = here;
            }while(fill != 0);
            incr = 1U << (len-1);
            while(huff&incr){
                incr >>= 1;
            }
            if(0<incr){
                huff &= incr - 1;
                huff += incr;
            }else{
                huff = 0;
            }
            ++sym;
            if(--count[len] == 0){
                if(len == max){
                    break;
                }
                len = lens[work[sym]];
            }

            if(root<len && (huff&mask) != low){
                if(drop == 0){
                    drop = root;
                }
                next += min;
                curr = len - drop;
                left = static_cast<s32>(1<<curr);
                while(curr + drop < max){
                    left -= count[curr+drop];
                    if(left<=0){
                        break;
                    }
                    ++curr;
                    left <<= 1;
                }

                used += 1U<<curr;
                if((LENS==type && ENOUGH_LENS<used)
                    || (DISTS==type && ENOUGH_DISTS<used)){
                    return 1;
                }
                low = huff & mask;
                (*table)[low].op_ = static_cast<u8>(curr);
                (*table)[low].bits_ = static_cast<u8>(root);
                (*table)[low].val_ = static_cast<u16>(next - *table);
            }
        }//for(;;)
        if(1<huff){
            here.op_ = 64;
            here.bits_ = static_cast<u8>(len-drop);
            here.val_ = 0;
            next[huff] = here;
        }
        *table += used;
        *bits = root;
        return 0;
    }

    /**
    */
    void inflate_fast(Stream& stream, u32 start)
    {
        InflateContext& context = *stream.inflate_;
        const u8* in = stream.next_in_;
        const u8* last = in + (stream.avail_in_-5);
        u8* out = stream.next_out_;
        u8* beg = out - (start-stream.avail_out_);
        u8* end = out + (stream.avail_out_ - 257);
        u32 wsize = context.wsize_;
        u32 whave = context.whave_;
        u32 wnext = context.wnext_;
        u8* window = context.window_;
        u32 hold = context.hold_;
        u32 bits = context.bits_;
        const CODE* lcode = context.lencode_;
        const CODE* dcode = context.distcode_;
        u32 lmask = (1U << context.lenbits_) - 1;
        u32 dmask = (1U << context.distbits_) - 1;
        u32 len;
        u32 op;
        u32 dist;
        u8* from;

        CODE here;
        do{
            if(bits<15){
                hold += (*in++) << bits;
                bits += 8;
                hold += (*in++) << bits;
                bits += 8;
            }
            here = lcode[hold & lmask];
            DOLEN:
            op = here.bits_;
            hold >>= op;
            bits -= op;
            op = here.op_;
            if(op<=0){
                *out++ = static_cast<u8>(here.val_);
            }else if(16 & op){
                len = here.val_;
                op &= 15;
                if(op){
                    if(bits<op){
                        hold += (*in++) << bits;
                        bits += 8;
                    }
                    len += hold & ((1U<<op)-1);
                    hold >>= op;
                    bits -= op;
                }
                if(bits<15){
                    hold += (*in++) << bits;
                    bits += 8;
                    hold += (*in++) << bits;
                    bits += 8;
                }
                here = dcode[hold&dmask];
                DODIST:
                op = here.bits_;
                hold >>= op;
                bits -= op;
                op = here.op_;
                if(16&op){
                    dist = here.val_;
                    op &= 15;
                    if(bits<op){
                        hold += (*in++) << bits;
                        bits += 8;
                        if(bits<op){
                            hold += (*in++) << bits;
                            bits += 8;
                        }
                    }
                    dist += hold & ((1U<<op)-1);
                    hold >>= op;
                    bits -= op;
                    op = static_cast<u32>(out-beg);
                    if(op<dist){
                        op = dist-op;
                        if(whave<op){
                            if(context.sane_){
                                context.mode_ = CPPZ_MODE_BAD;
                                break;
                            }
                        }
                        from = window;
                        if(0 == wnext){
                            from += wsize - op;
                            if(op<len){
                                len -= op;
                                do{
                                    *out++ = *from++;
                                }while(--op);
                                from = out - dist;
                            }
                        }else if(wnext < op){
                            from += wsize + wnext - op;
                            op -= wnext;
                            if(op<len){
                                len -= op;
                                do{
                                    *out++ = *from++;
                                }while(--op);
                                from = window;
                                if(wnext<len){
                                    op = wnext;
                                    len -= op;
                                    do{
                                        *out++ = *from++;
                                    }while(--op);
                                    from = out - dist;
                                }
                            }
                        }else{
                            from += wnext - op;
                            if(op<len){
                                len -= op;
                                do{
                                    *out++ = *from++;
                                }while(--op);
                                from = out - dist;
                            }
                        } //if(0 == wnext)
                        while(2<len){
                            *out++ = *from++;
                            *out++ = *from++;
                            *out++ = *from++;
                            len -= 3;
                        }
                        if(len){
                            *out++ = *from++;
                            if(1<len){
                                *out++ = *from++;
                            }
                        }
                    }else{ //(if(op<dist)
                        from = out-dist;
                        do{
                            *out++ = *from++;
                            *out++ = *from++;
                            *out++ = *from++;
                            len -= 3;
                        }while(2<len);
                        if(len){
                            *out++ = *from++;
                            if(1<len){
                                *out++ = *from++;
                            }
                        }
                    }//if(op<dist)
                } else if((64&op)==0){//if(16&op)
                    here = dcode[here.val_ + (hold & ((1U<<op)-1))];
                    goto DODIST;
                }else{
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                } //if(16&op)
            }else if((64&op) == 0){
                here = lcode[here.val_ + (hold & ((1U<<op)-1))];
                goto DOLEN;
            }else if(32&op){
                context.mode_ = CPPZ_MODE_TYPE;
                break;
            }else {
                context.mode_ = CPPZ_MODE_BAD;
                break;
            }//if(op<=0)
        }while(in<last && out<end);

        len = bits>>3;
        in -= len;
        bits -= len<<3;
        hold &= (1U<<bits) - 1;

        stream.next_in_ = in;
        stream.next_out_ = out;
        stream.avail_in_ = (in<last)? 5 + static_cast<u32>(last-in) : 5 - static_cast<u32>(in-last);
        stream.avail_out_ = (out<end)
            ? 257 + static_cast<u32>(end-out)
            : 257 - static_cast<u32>(out-end);
        context.hold_ = hold;
        context.bits_ = bits;
    }
}

    //--------------------------------------------------------------------
    //---
    //--- Exported functions
    //---
    //--------------------------------------------------------------------
    s32 cppz_deflateReset(Stream& stream)
    {
        if(deflateStateCheck(stream)){
            return CPPZ_STREAM_ERROR;
        }

        stream.total_in_ = 0;
        stream.total_out_ = 0;

        DeflateContext& context = *stream.deflate_;
        context.pending_ = 0;
        context.pendingOut_ = context.pending_buff_;

        if(context.wrap_ < 0){
            context.wrap_ = -context.wrap_; // was made negative by deflate(..., Z_FINISH);
        }

        context.state_ = (context.wrap_)? State_Init : State_Busy;

        context.high_water_ = 0;

        stream.adler32_ = adler32(0, CPPZ_NULL, 0);

        context.last_flush_ = CPPZ_NO_FLUSH;
        tree_init(context.treeContext_);
        longestMatch_init(context);
        return CPPZ_OK;
    }

    s32 cppz_deflateInit(Stream& stream, cppz_alloc_func alloc_func, cppz_free_func free_func, void* user)
    {
        memset(&stream, 0, sizeof(Stream));

        stream.alloc_func_ = (CPPZ_NULL==alloc_func)? cppz_alloc : alloc_func;
        stream.free_func_ = (CPPZ_NULL==free_func)? cppz_free : free_func;
        stream.user_ = user;

        s32 wrap=1;
        s32 level = 6;
        s32 window_bits = CPPZ_DEFAULT_WINDOW_BITS;
        s32 memlevel = CPPZ_DEFAULT_MEM_LEVEL;
        s32 strategy = CPPZ_DEFAULT_STRATEGY;

        stream.deflate_ = (DeflateContext*)stream.alloc_func_(sizeof(DeflateContext), stream.user_);
        if(CPPZ_NULL == stream.deflate_){
            return CPPZ_MEM_ERROR;
        }

        if(window_bits < 0){ // suppress zlib wrapper
            wrap = 0;
            window_bits = -window_bits;
        }

        DeflateContext& context = *stream.deflate_;
        context.state_ = State_Init;

        context.wrap_ = wrap;
        context.w_bits_ = window_bits;
        context.w_size_ = 1<<window_bits;
        context.w_mask_ = stream.deflate_->w_size_-1;
        context.hash_bits_ = memlevel + 7;
        context.hash_size_ = 1<<stream.deflate_->hash_bits_;
        context.hash_mask_ = stream.deflate_->hash_size_-1;
        context.hash_shift_ =  (stream.deflate_->hash_bits_+MIN_MATCH-1)/MIN_MATCH;

        context.lit_bufsize_ = 1<<(memlevel+6);

        context.level_ = level;
        context.strategy_ = strategy;
        return cppz_deflateReset(stream);
    }

    s32 cppz_deflate(Stream& stream, s32 flush)
    {
        CPPZ_ASSERT(CPPZ_NULL != stream.deflate_);
        CPPZ_ASSERT(CPPZ_NULL != stream.next_out_);
        CPPZ_ASSERT(0<=flush && flush<=CPPZ_BLOCK);

        if(deflateStateCheck(stream)){
            return CPPZ_STREAM_ERROR;
        }
        DeflateContext& context = *stream.deflate_;

        if(!stream.avail_out_){
            return CPPZ_BUF_ERROR;
        }
        if(CPPZ_NULL == stream.next_out_
            || (0 < stream.avail_in_ && CPPZ_NULL == stream.next_in_)
            || (State_Finish == context.state_ && CPPZ_FINISH != flush)){
            return CPPZ_STREAM_ERROR;
        }
        if(stream.avail_out_<=0){
            return CPPZ_BUF_ERROR;
        }

        s32 old_flush = context.last_flush_;
        context.last_flush_ = flush;

        // Flush as much pending output as possible
        if(0 < context.pending_){
            flush_pending(stream);
            if(stream.avail_out_<=0){
                /* Since avail_out is 0, deflate will be called again with
                * more output space, but possibly with both pending and
                * avail_in equal to zero. There won't be anything to do,
                * but this is not an error situation so make sure we
                * return OK instead of BUF_ERROR at next call of deflate:
                */
                context.last_flush_ = -1;
                return CPPZ_OK;
            }

            /* Make sure there is something to do and avoid duplicate consecutive
            * flushes. For repeated and useless calls with Z_FINISH, we keep
            * returning Z_STREAM_END instead of Z_BUF_ERROR.
            */
        }else if(stream.avail_in_<=0
            && rank(flush) <= rank(old_flush)
            && CPPZ_FINISH != flush){
            return CPPZ_BUF_ERROR;
        }

        // User must not provide more input after the first FINISH:
        if(State_Finish == context.state_ && 0<stream.avail_in_){
            return CPPZ_BUF_ERROR;
        }

        // Write the header
        if(State_Init == context.state_){
            u32 header = (CPPZ_DEFLATED + ((context.w_bits_-8)<<4)) << 8;
            u32 level_flags;
            if(CPPZ_HUFFMAN_ONLY<=context.strategy_ || context.level_<2){
                level_flags = 0;
            }else if(context.level_<CPPZ_DEFAULT_LEVEL){
                level_flags = 1;
            }else if(CPPZ_DEFAULT_LEVEL == context.level_){
                level_flags = 2;
            }else{
                level_flags = 3;
            }
            header |= (level_flags<<6);
            if(0 != context.strstart_){
                header |= CPPZ_PRESET_DICT;
            }
            header += 31 - (header % 31);

            putShortMSB(context, header);
            // Save the adler32 of the preset dictionary:
            if(0 != context.strstart_){
                putShortMSB(context, (stream.adler32_>>16));
                putShortMSB(context, (stream.adler32_ & 0xFFFFU));
            }
            stream.adler32_ = adler32(0UL, CPPZ_NULL, 0);
            context.state_ = State_Busy;

            // Compression must start with an empty pending buffer
            flush_pending(stream);
            if(0 != context.pending_){
                context.last_flush_ = -1;
                return CPPZ_OK;
            }
        }

        // Start a new block or continue the current one.
        if(0<stream.avail_in_
            || 0<context.lookahead_
            || (CPPZ_NO_FLUSH != flush && State_Finish != context.state_)){
            BlockState blockState;
            blockState = deflate_slow(stream, flush);

            if(BlockState_FinishStarted == blockState || BlockState_FinishDone == blockState){
                context.state_ = State_Finish;
            }
            // If flush != Z_NO_FLUSH && avail_out == 0, the next call of deflate should use the same flush parameter to make sure
            // that the flush is complete. So we don't have to output an empty block here, this will be done at next call. This also
            // ensures that for a very small output buffer, we emit at most one empty block.
            if(BlockState_NeedMore == blockState || BlockState_FinishStarted == blockState){
                if(stream.avail_out_<=0){
                    context.last_flush_ = -1;
                }
                return CPPZ_OK;
            }
            if(BlockState_BlockDone == blockState){
                if(CPPZ_PARTIAL_FLUSH == flush){ //FULL_FLUSH or SYNC_FLUSH
                    tree_align(context);
                }else if(CPPZ_BLOCK != flush){
                    tree_stored_block(context, CPPZ_NULL, 0UL, 0);
                    // For a full flush, this empty block will be recognized as a special marker by inflate_sync().
                    if(CPPZ_FULL_FLUSH == flush){
                        clear_hash(context); //forget history
                        if(context.lookahead_<=0){
                            context.strstart_ = 0;
                            context.block_start_ = 0;
                            context.insert_ = 0;
                        }
                    }
                }
                flush_pending(stream);
                if(stream.avail_out_<=0){
                    context.last_flush_ = -1;
                    return CPPZ_OK;
                }
            } //if(BlockState_BlockDone == blockState
        } //if(0<stream.avail_in_

        if(CPPZ_FINISH != flush){
            return CPPZ_OK;
        }
        if(context.wrap_<=0){
            return CPPZ_STREAM_END;
        }

        putShortMSB(context, (stream.adler32_>>16));
        putShortMSB(context, (stream.adler32_&0xFFFFU));
        flush_pending(stream);

        // If avail_out is zero, the application will call deflate again to flush the rest.
        if(0<context.wrap_){
            context.wrap_ = -context.wrap_;
        }
        return 0<context.pending_? CPPZ_OK : CPPZ_STREAM_END;
    }

    s32 cppz_deflateEnd(Stream& stream)
    {
        if(deflateStateCheck(stream)){
            return CPPZ_STREAM_ERROR;
        }
        if(CPPZ_NULL != stream.deflate_){
            s32 state = stream.deflate_->state_;
            stream.free_func_(stream.deflate_, stream.user_);
            stream.deflate_ = CPPZ_NULL;
            stream.free_func_ = CPPZ_NULL;
            stream.alloc_func_ = CPPZ_NULL;
            stream.user_ = CPPZ_NULL;
            return state == State_Busy? CPPZ_DATA_ERROR : CPPZ_OK;
        }else{
            return CPPZ_OK;
        }
    }


    s32 cppz_inflateReset(Stream& stream)
    {
        if(inflateStateCheck(stream)){
            return CPPZ_STREAM_ERROR;
        }
        stream.total_in_ = 0;
        stream.total_out_ = 0;

        s32 windowBits = CPPZ_DEFAULT_WINDOW_BITS;
        s32 wrap;
        if(windowBits<0){
            wrap = 0;
            windowBits = -windowBits;
        }else{
            wrap = (windowBits>>4) + 5;
        }

        if(windowBits && (windowBits < 8 || 15<windowBits)){
            return CPPZ_STREAM_ERROR;
        }

        InflateContext& context = *stream.inflate_;
        context.wrap_ = wrap;
        context.wbits_ = static_cast<u32>(windowBits);

        context.wsize_ = 0;
        context.whave_ = 0;
        context.wnext_ = 0;

        if(context.wrap_){
            stream.adler32_ = context.wrap_ & 0x01U;
        }

        context.mode_ = CPPZ_MODE_HEAD;
        context.last_ = 0;
        context.havedict_ = 0;
        context.dmax_ = CPPZ_DICTIONARY_SIZE;
        context.hold_ = 0;
        context.bits_ = 0;
        context.lencode_ = context.distcode_ = context.next_ = context.codes_;
        context.sane_ = 1;
        context.back_ = -1;
        return CPPZ_OK;
    }

    s32 cppz_inflateInit(Stream& stream, cppz_alloc_func alloc_func, cppz_free_func free_func, void* user)
    {
        memset(&stream, 0, sizeof(Stream));

        stream.alloc_func_ = (CPPZ_NULL==alloc_func)? cppz_alloc : alloc_func;
        stream.free_func_ = (CPPZ_NULL==free_func)? cppz_free : free_func;
        stream.user_ = user;

        stream.inflate_ = (InflateContext*)stream.alloc_func_(sizeof(InflateContext), stream.user_);
        if(CPPZ_NULL == stream.inflate_){
            return CPPZ_MEM_ERROR;
        }

        InflateContext& context = *stream.inflate_;
        context.mode_ = CPPZ_MODE_HEAD;

        return cppz_inflateReset(stream);
    }

    s32 cppz_inflate(Stream& stream, s32 flush)
    {
        CPPZ_ASSERT(CPPZ_NULL != stream.next_out_);
        CPPZ_ASSERT(CPPZ_NULL != stream.next_in_ || stream.avail_in_<=0);
        static const u16 order[19] = //permutation of code lengths
        {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

        if(inflateStateCheck(stream)){
            return CPPZ_STREAM_ERROR;
        }
        InflateContext& context = *stream.inflate_;
        if(CPPZ_MODE_TYPE == context.mode_){
            context.mode_ = CPPZ_MODE_TYPEDO; //skip check
        }

        const u8* next;
        u8* put;
        u32 left;
        u32 have;
        u32 hold;
        u32 bits;
        load(next, put, left, have, hold, bits, stream);
        u32 in = have;
        u32 out = left;
        u32 len;
        s32 ret = CPPZ_OK;
        CODE here;
        u32 copy = 0;

        for(;;){
            switch(context.mode_){
            case CPPZ_MODE_HEAD:
                if(0 == context.wrap_){
                    context.mode_ = CPPZ_MODE_TYPEDO;
                    break;
                }
                NEEDBITS(next, have, hold, bits, 16);
                if( ((get_bits(hold, 8)<<8) + (hold>>8))%31 ){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                if(get_bits(hold, 4) != CPPZ_DEFLATED){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                dropbits(hold, bits, 4);
                len = get_bits(hold, 4) + 8;
                if(0 == context.wbits_){
                    context.wbits_ = len;
                }
                if(15<len || context.wbits_<len){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                context.dmax_ = 0x1U<<len;
                stream.adler32_ = context.check_ = adler32(0, CPPZ_NULL, 0);
                context.mode_ = hold & 0x200? CPPZ_MODE_DICTID : CPPZ_MODE_TYPE;
                initbits(hold, bits);
                break;
            case CPPZ_MODE_DICTID:
                NEEDBITS(next, have, hold, bits, 32);
                stream.adler32_ = context.check_ = reverse(hold);
                initbits(hold, bits);
                context.mode_ = CPPZ_MODE_DICT;
            case CPPZ_MODE_DICT:
                if(0==context.havedict_){
                    restore(stream, put, left, next, have, hold, bits);
                    return CPPZ_NEED_DICT;
                }
                stream.adler32_ = context.check_ = adler32(0U, CPPZ_NULL, 0);
                context.mode_ = CPPZ_MODE_TYPE;
            case CPPZ_MODE_TYPE:
                if(CPPZ_BLOCK==flush || CPPZ_TREES==flush){
                    goto INF_LEAVE;
                }
            case CPPZ_MODE_TYPEDO:
                if(context.last_){
                    bytebits(hold, bits);
                    context.mode_ = CPPZ_MODE_CHECK;
                    break;
                }
                NEEDBITS(next, have, hold, bits, 3);
                context.last_ = get_bits(hold, 1);
                dropbits(hold, bits, 1);
                switch(get_bits(hold, 2)){
                case 0:
                    context.mode_ = CPPZ_MODE_STORED;
                    break;
                case 1:
                    fixedtables(context);
                    context.mode_ = CPPZ_MODE_LEN;
                    if(CPPZ_TREES == flush){
                        dropbits(hold, bits, 2);
                        goto INF_LEAVE;
                    }
                    break;
                case 2:
                    context.mode_ = CPPZ_MODE_TABLE;
                    break;
                case 3:
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                dropbits(hold, bits, 2);
                break;
            case CPPZ_MODE_STORED:
                bytebits(hold, bits); //go to byte boundary
                NEEDBITS(next, have, hold, bits, 32);
                if((hold&0xFFFFU) != ((hold>>16)^0xFFFFU)){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                context.length_ = hold & 0xFFFFU;
                initbits(hold, bits);
                context.mode_ = CPPZ_MODE_COPY_;
                if(CPPZ_TREES == flush){
                    goto INF_LEAVE;
                }
            case CPPZ_MODE_COPY_:
                context.mode_ = CPPZ_MODE_COPY;
            case CPPZ_MODE_COPY:
                copy = context.length_;
                if(copy){
                    if(have<copy){
                        copy = have;
                    }
                    if(left<copy){
                        copy = left;
                    }
                    if(0 == copy){
                        goto INF_LEAVE;
                    }
                    memcpy(put, next, copy);
                    have -= copy;
                    next += copy;
                    left -= copy;
                    put += copy;
                    context.length_ -= copy;
                    break;
                }
                context.mode_ = CPPZ_MODE_TYPE;
                break;
            case CPPZ_MODE_TABLE:
                NEEDBITS(next, have, hold, bits, 14);
                context.nlen_ = get_bits(hold, 5) + 257;
                dropbits(hold, bits, 5);
                context.ndist_ = get_bits(hold, 5) + 1;
                dropbits(hold, bits, 5);
                context.ncode_ = get_bits(hold, 4) + 4;
                dropbits(hold, bits, 4);
                context.have_ = 0;
                context.mode_ = CPPZ_MODE_LENLENS;
            case CPPZ_MODE_LENLENS:
                while(context.have_<context.ncode_){
                    NEEDBITS(next, have, hold, bits, 3);
                    context.lens_[ order[context.have_++] ] = static_cast<u16>(get_bits(hold, 3));
                    dropbits(hold, bits, 3);
                }
                while(context.have_<19){
                    context.lens_[ order[context.have_++] ] = 0;
                }
                context.next_ = context.codes_;
                context.lencode_ = context.next_;
                context.lenbits_ = 7;
                ret = inflate_table(CODES, context.lens_, 19, &(context.next_), &(context.lenbits_), context.work_);
                if(ret){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                context.have_ = 0;
                context.mode_ = CPPZ_MODE_CODELENS;
            case CPPZ_MODE_CODELENS:
                while(context.have_ < (context.nlen_+context.ndist_)){
                    for(;;){
                        here = context.lencode_[get_bits(hold, context.lenbits_)];
                        if(here.bits_<=bits){
                            break;
                        }
                        PULLBYTE(next, have, hold, bits);
                    }
                    if(here.val_<16){
                        dropbits(hold, bits, here.bits_);
                        context.lens_[context.have_++] = here.val_;
                    }else{
                        if(16==here.val_){
                            NEEDBITS(next,have, hold, bits, here.bits_+2);
                            dropbits(hold, bits, here.bits_);
                            if(0==context.have_){
                                context.mode_ = CPPZ_MODE_BAD;
                                break;
                            }
                            len = context.lens_[context.have_-1];
                            copy = 3 + get_bits(hold, 2);
                            dropbits(hold, bits, 2);
                        }else if(17==here.val_){
                            NEEDBITS(next, have, hold, bits, here.bits_+3);
                            dropbits(hold, bits, here.bits_);
                            len=0;
                            copy = 3 + get_bits(hold, 3);
                            dropbits(hold, bits, 3);
                        }else{
                            NEEDBITS(next, have, hold, bits, here.bits_+7);
                            dropbits(hold, bits, here.bits_);
                            len = 0;
                            copy = 11 + get_bits(hold, 7);
                            dropbits(hold, bits, 7);
                        }
                        if((context.nlen_+context.ndist_) < (context.have_+copy)){
                            context.mode_ = CPPZ_MODE_BAD;
                            break;
                        }
                        while(copy--){
                            context.lens_[context.have_++] = static_cast<u16>(len);
                        }
                    }//if(here.val_
                }//while(context.have_
                //handle error breaks in while
                if(CPPZ_MODE_BAD == context.mode_){
                    break;
                }
                //check for end-of-block code (better have one)
                if(0 == context.lens_[256]){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                //build code tables
                context.next_ = context.codes_;
                context.lencode_ = context.next_;
                context.lenbits_ = 9;
                ret = inflate_table(LENS, context.lens_, context.nlen_, &(context.next_), &(context.lenbits_), context.work_);
                if(ret){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                context.distcode_ = context.next_;
                context.distbits_ = 6;
                ret = inflate_table(DISTS, context.lens_+context.nlen_, context.ndist_, &(context.next_), &(context.distbits_), context.work_);
                if(ret){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                context.mode_ = CPPZ_MODE_LEN;
                if(CPPZ_TREES == flush){
                    goto INF_LEAVE;
                }

            case CPPZ_MODE_LEN_:
                context.mode_ = CPPZ_MODE_LEN;
            case CPPZ_MODE_LEN:
                if(6<=have && 258<=left){
                    restore(stream, put, left, next, have, hold, bits);
                    inflate_fast(stream, out);
                    load(next, put, left, have, hold, bits, stream);
                    if(CPPZ_MODE_TYPE == context.mode_){
                        context.back_ = -1;
                    }
                    break;
                }
                context.back_ = 0;
                for(;;){
                    here = context.lencode_[get_bits(hold, context.lenbits_)];
                    if(here.bits_<=bits){
                        break;
                    }
                    PULLBYTE(next, have, hold, bits);
                }
                if(here.op_ && 0==(here.op_&0xF0U)){
                    CODE last = here;
                    for(;;){
                        here = context.lencode_[last.val_ + (get_bits(hold, last.bits_+last.op_)>>last.bits_)];
                        if(static_cast<u32>(last.bits_+here.bits_)<=bits){
                            break;
                        }
                        PULLBYTE(next, have, hold, bits);
                    }
                    dropbits(hold, bits, last.bits_);
                    context.back_ += last.bits_;
                }
                dropbits(hold, bits, here.bits_);
                context.back_ += here.bits_;
                context.length_ = here.val_;
                if(0 == here.op_){
                    context.mode_ = CPPZ_MODE_LIT;
                    break;
                }
                if(32 & here.op_){
                    context.back_ = -1;
                    context.mode_ = CPPZ_MODE_TYPE;
                    break;
                }
                if(64 & here.op_){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                context.extra_ = here.op_&15;
                context.mode_ = CPPZ_MODE_LENEXT;
            case CPPZ_MODE_LENEXT:
                if(context.extra_){
                    NEEDBITS(next, have, hold, bits, context.extra_);
                    context.length_ += get_bits(hold, context.extra_);
                    dropbits(hold, bits, context.extra_);
                    context.back_ += context.extra_;
                }
                context.was_ = context.length_;
                context.mode_ = CPPZ_MODE_DIST;
            case CPPZ_MODE_DIST:
                for(;;){
                    here = context.distcode_[get_bits(hold, context.distbits_)];
                    if(here.bits_<=bits){
                        break;
                    }
                    PULLBYTE(next, have, hold, bits);
                }
                if(0 == (here.op_&0xF0U)){
                    CODE last = here;
                    for(;;){
                        here = context.distcode_[last.val_ + (get_bits(hold, last.bits_+last.op_)>>last.bits_)];
                        if(static_cast<u32>(last.bits_+here.bits_)<=bits){
                            break;
                        }
                        PULLBYTE(next, have, hold, bits);
                    }
                    dropbits(hold, bits, last.bits_);
                    context.back_ += last.bits_;
                }
                dropbits(hold, bits, here.bits_);
                context.back_ += here.bits_;
                if(64 & here.op_){
                    context.mode_ = CPPZ_MODE_BAD;
                    break;
                }
                context.offset_ = here.val_;
                context.extra_ = here.op_ & 15;
                context.mode_ = CPPZ_MODE_DISTEXT;

            case CPPZ_MODE_DISTEXT:
                if(context.extra_){
                    NEEDBITS(next, have, hold, bits, context.extra_);
                    context.offset_ += get_bits(hold, context.extra_);
                    dropbits(hold, bits, context.extra_);
                    context.back_ += context.extra_;
                }
                context.mode_ = CPPZ_MODE_MATCH;
            case CPPZ_MODE_MATCH:
                if(0==left){
                    goto INF_LEAVE;
                }
                u8* from;
                copy = out - left;
                if(copy<context.offset_){
                    copy = context.offset_ - copy;
                    if(context.whave_<copy){
                        if(context.sane_){
                            context.mode_ = CPPZ_MODE_BAD;
                            break;
                        }
                    }
                    if(context.wnext_<copy){
                        copy -= context.wnext_;
                        from = context.window_ + (context.wsize_ - copy);
                    }else{
                        from = context.window_ + (context.wnext_ - copy);
                    }
                    if(context.length_<copy){
                        copy = context.length_;
                    }
                }else{
                    from = put - context.offset_;
                    copy = context.length_;
                }
                if(left<copy){
                    copy = left;
                }
                left -= copy;
                context.length_ -= copy;
                do{
                    *put++ = *from++;
                }while(--copy);
                if(0 == context.length_){
                    context.mode_ = CPPZ_MODE_LEN;
                }
                break;
            case CPPZ_MODE_LIT:
                if(0==left){
                    goto INF_LEAVE;
                }
                *put++ = static_cast<u8>(context.length_);
                --left;
                context.mode_ = CPPZ_MODE_LEN;
                break;
            case CPPZ_MODE_CHECK:
                if(context.wrap_){
                    NEEDBITS(next, have, hold, bits, 32);
                    out -= left;
                    stream.total_out_ += out;
                    context.total_ += out;
                    if((4 & context.wrap_) && out){
                        stream.adler32_ = context.check_ = adler32(context.check_, put-out, out);
                    }
                    out = left;
                    if((4&context.wrap_) && (reverse(hold) != context.check_)){
                        context.mode_ = CPPZ_MODE_BAD;
                        break;
                    }
                    initbits(hold, bits);
                }
                context.mode_ = CPPZ_MODE_DONE;
            case CPPZ_MODE_DONE:
                ret = CPPZ_STREAM_END;
                goto INF_LEAVE;
            case CPPZ_MODE_BAD:
                ret = CPPZ_DATA_ERROR;
                goto INF_LEAVE;
            case CPPZ_MODE_MEM:
                return CPPZ_MEM_ERROR;
            case CPPZ_MODE_SYNC:
            default:
                return CPPZ_STREAM_ERROR;
            }
        } //for(;;)
        /*
        Return from inflate(), updating the total counts and the check value.
        If there was no progress during the inflate() call, return a buffer
        error.  Call updatewindow() to create and/or update the window state.
        Note: a memory error from inflate() is non-recoverable.
        */
INF_LEAVE:
        restore(stream, put, left, next, have, hold, bits);
        if(context.wsize_
            || (out != stream.avail_out_ && context.mode_<CPPZ_MODE_BAD && (context.mode_<CPPZ_MODE_CHECK || flush != CPPZ_FINISH))){
            if(updatewindow(stream, stream.next_out_, out-stream.avail_out_)){
                context.mode_ = CPPZ_MODE_MEM;
                return CPPZ_MEM_ERROR;
            }
        }
        in -= stream.avail_in_;
        out -= stream.avail_out_;
        stream.total_in_ += in;
        stream.total_out_ += out;
        context.total_ += out;
        if((4&context.wrap_) && out){
            stream.adler32_ = context.check_ = adler32(context.check_, stream.next_out_-out, out);
        }
        if(((0==in && 0==out) || CPPZ_FINISH==flush) && ret == CPPZ_OK){
            ret = CPPZ_BUF_ERROR;
        }
        return ret;
    }

    //
    s32 cppz_inflateEnd(Stream& stream)
    {
        s32 result = inflateStateCheck(stream);
        if(CPPZ_NULL != stream.inflate_){
            stream.free_func_(stream.inflate_, stream.user_);
            stream.inflate_ = CPPZ_NULL;
            stream.free_func_ = CPPZ_NULL;
            stream.alloc_func_ = CPPZ_NULL;
            stream.user_ = CPPZ_NULL;
        }
        return result;
    }
#endif
}
#endif //INC_CPPZ_H_
