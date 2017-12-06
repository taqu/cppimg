#include "catch.hpp"
#include "../deflate.h"

//#define CPPZ_TEST_ZLIB
#ifdef CPPZ_TEST_ZLIB
#include "zlib.h"
#endif
#include <string.h>
#include <random>

//#define CPPZ_OUT_LOG
//#define CPPZ_TEST_VERY_LARGE

#ifdef CPPZ_TEST_ZLIB
int def(cppz::u8* dst, cppz::u32 srcSize, cppz::u8* src, int level)
{
    int ret, flush;
    z_stream stream;
    stream.zalloc = NULL;
    stream.zfree = NULL;
    stream.opaque = NULL;
    ret = deflateInit(&stream, level);
    if(Z_OK != ret){
        return ret;
    }

    const int Chunk = 16384;
    cppz::u8 in[Chunk];
    cppz::u8 out[Chunk];
    cppz::u32 count = 0;
    int outCount = 0;
    do{
        if(srcSize<=count){
            deflateEnd(&stream);
            return outCount;
        }
        cppz::u32 size = Chunk;
        if(srcSize<(count+size)){
            size = srcSize-count;
        }
        stream.avail_in = size;
        memcpy(in, src+count, size);
        count += size;
        flush = (srcSize<=count)? Z_FINISH : Z_NO_FLUSH;
        stream.next_in = in;
        do{
            stream.avail_out = Chunk;
            stream.next_out = out;
            ret = deflate(&stream, flush);
            int s = Chunk - stream.avail_out;
            memcpy(dst+outCount, out, s);
            outCount += s;
        }while(stream.avail_out == 0);
    }while(flush != Z_FINISH);
    deflateEnd(&stream);
    return outCount;
}

int inf(cppz::u8* dst, cppz::u32 srcSize, cppz::u8* src)
{
    int ret;
    z_stream stream;
    stream.zalloc = NULL;
    stream.zfree = NULL;
    stream.opaque = NULL;
    ret = inflateInit(&stream);
    if(Z_OK != ret){
        return ret;
    }

    const int Chunk = 16384;
    cppz::u8 in[Chunk];
    cppz::u8 out[Chunk];
    cppz::u32 count = 0;
    int outCount = 0;
    do{
        if(srcSize<=count){
            inflateEnd(&stream);
            return Z_OK;
        }
        cppz::u32 size = Chunk;
        if(srcSize<(count+size)){
            size = srcSize-count;
        }
        stream.avail_in = size;
        memcpy(in, src+count, size);
        count += size;
        stream.next_in = in;

        do{
            stream.avail_out = Chunk;
            stream.next_out = out;
            ret = inflate(&stream, Z_NO_FLUSH);
            switch(ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
                break;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&stream);
                return ret;
            }
            int s = Chunk - stream.avail_out;
            memcpy(dst+outCount, out, s);
            outCount += s;
        } while(stream.avail_out == 0);
    } while(ret != Z_STREAM_END);
    inflateEnd(&stream);
    return ret == Z_STREAM_END? outCount : Z_DATA_ERROR;
}
#endif //CPPZ_TEST_ZLIB

int def2(cppz::u8* dst, cppz::u32 srcSize, cppz::u8* src)
{
    int ret, flush;
    cppz::Stream stream;
    ret = cppz::cppz_deflateInit(stream);
    if(cppz::CPPZ_OK != ret){
        return ret;
    }

    const int Chunk = 16384;
    cppz::u8 in[Chunk];
    cppz::u8 out[Chunk];
    cppz::u32 count = 0;
    int outCount = 0;
    do{
        if(srcSize<=count){
            cppz::cppz_deflateEnd(stream);
            return outCount;
        }
        cppz::u32 size = Chunk;
        if(srcSize<(count+size)){
            size = srcSize-count;
        }
        stream.avail_in_ = size;
        memcpy(in, src+count, size);
        count += size;
        flush = (srcSize<=count)? cppz::CPPZ_FINISH : cppz::CPPZ_NO_FLUSH;
        stream.next_in_ = in;
        do{
            stream.avail_out_ = Chunk;
            stream.next_out_ = out;
            ret = cppz_deflate(stream, flush);
            int s = Chunk - stream.avail_out_;
            memcpy(dst+outCount, out, s);
            outCount += s;
        }while(stream.avail_out_ == 0);
    }while(flush != cppz::CPPZ_FINISH);
    cppz_deflateEnd(stream);
    return outCount;
}

int inf2(cppz::u8* dst, cppz::u32 srcSize, cppz::u8* src)
{
    int ret;
    cppz::Stream stream;
    ret = cppz::cppz_inflateInit(stream);
    if(cppz::CPPZ_OK != ret){
        return ret;
    }

    const int Chunk = 16384;
    cppz::u8 in[Chunk];
    cppz::u8 out[Chunk];
    cppz::u32 count = 0;
    int outCount = 0;
    do{
        if(srcSize<=count){
            cppz::cppz_inflateEnd(stream);
            return cppz::CPPZ_OK;
        }
        cppz::u32 size = Chunk;
        if(srcSize<(count+size)){
            size = srcSize-count;
        }
        stream.avail_in_ = size;
        memcpy(in, src+count, size);
        count += size;
        stream.next_in_ = in;

        do{
            stream.avail_out_ = Chunk;
            stream.next_out_ = out;
            ret = cppz::cppz_inflate(stream, cppz::CPPZ_NO_FLUSH);
            switch(ret)
            {
            case cppz::CPPZ_NEED_DICT:
                ret = cppz::CPPZ_DATA_ERROR;
                break;
            case cppz::CPPZ_DATA_ERROR:
            case cppz::CPPZ_MEM_ERROR:
                cppz::cppz_inflateEnd(stream);
                return ret;
            }
            int s = Chunk - stream.avail_out_;
            memcpy(dst+outCount, out, s);
            outCount += s;
        } while(stream.avail_out_ == 0);
    } while(ret != cppz::CPPZ_STREAM_END);
    cppz::cppz_inflateEnd(stream);
    return ret == cppz::CPPZ_STREAM_END? outCount : cppz::CPPZ_DATA_ERROR;
}

void test(int loopCount, cppz::u32 size)
{
    //size = 1048576;
    cppz::u32 seed_;
    cppz::u32 length;
    std::mt19937 mt;
    std::random_device rand;
    const cppz::u32 MaxSource = size;
    cppz::u8* compressed0 = reinterpret_cast<cppz::u8*>(malloc(MaxSource*3));
    cppz::u8* decompressed0 = reinterpret_cast<cppz::u8*>(malloc(MaxSource));

    cppz::u8* compressed1 = reinterpret_cast<cppz::u8*>(malloc(MaxSource*3));
    cppz::u8* decompressed1 = reinterpret_cast<cppz::u8*>(malloc(MaxSource));
    cppz::u8* source = reinterpret_cast<cppz::u8*>(malloc(MaxSource));

    std::uniform_int_distribution<cppz::u32> dist(MaxSource/2, MaxSource);
    std::uniform_int_distribution<cppz::u32> dist_byte(0, 255);
    for(int i=0; i<loopCount; ++i){
        seed_ = rand();
        mt.seed(seed_);
#ifdef CPPZ_OUT_LOG
        printf("seed %u\n", seed_);
#endif

        length = dist(mt);
        for(cppz::u32 j=0; j<length; ++j){
            source[j] = static_cast<cppz::u8>(dist_byte(mt));
        }

#ifdef CPPZ_OUT_LOG
        printf("source length: %d\n", length);
#endif
        cppz::u32 count;
#ifdef CPPZ_TEST_ZLIB
        int ret0_0 = def(compressed0, length, source, 9);
        int ret0_1 = inf(decompressed0, ret0_0, compressed0);
#ifdef CPPZ_OUT_LOG
        printf(" zlib\n");
        printf("  compress: %s %d\n", (0<=ret0_0)?"OK":"NG", ret0_0);
        printf("  decompress: %s %d\n", (0<=ret0_1)?"OK":"NG", ret0_1);
        printf("  length check: %s\n", (length==ret0_1)?"OK":"NG");
        printf("  content check: ");
#endif
        for(count=0; count<length; ++count){
            if(decompressed0[count] != source[count]){
                break;
            }
        }
#ifdef CPPZ_OUT_LOG
        printf("%s\n", count==length?"OK":"NG");
#endif
#endif //CPPZ_TEST_ZLIB

        int ret1_0 = def2(compressed1, length, source);
        int ret1_1 = inf2(decompressed1, ret1_0, compressed1);
#ifdef CPPZ_OUT_LOG
        printf(" cppz\n");
        printf("  compress: %s %d\n", (0<=ret1_0)?"OK":"NG", ret1_0);
        printf("  decompress: %s %d\n", (0<=ret1_1)?"OK":"NG", ret1_1);
        printf("  length check: %s\n", (length==ret1_1)?"OK":"NG");
        printf("  content check: ");
#endif
        for(count=0; count<length; ++count){
            if(decompressed1[count] != source[count]){
                break;
            }
        }
#ifdef CPPZ_OUT_LOG
        printf("%s\n", count==length?"OK":"NG");
#else
        if(count != length){
            printf("NG at size %u, seed %u, length %d\n", size, seed_, length);
        }
#endif
        REQUIRE(static_cast<cppz::u32>(ret1_1) == length);
        REQUIRE(count == length);
    }

    free(source);
    free(decompressed1);
    free(compressed1);
    free(decompressed0);
    free(compressed0);
}

TEST_CASE("deflate/inflate random data" "[Defalate]")
{
    static const int Count = 64;
    for(int i=8; i<=256; i+=8){
        test(Count, i);
    }
    for(int i=128; i<=1024; i+=128){
        test(Count, i);
    }
    for(int i=1024*4; i<=1024*16; i+=1024){
        test(Count, i);
    }
#ifdef CPPZ_TEST_VERY_LARGE
    for(int i=1024*1024; i<=1024*1024*4; i+=1024*1024){
        test(Count, i);
    }
#endif
}

