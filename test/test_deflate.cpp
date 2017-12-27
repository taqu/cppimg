#include "catch.hpp"
#include "../cppimg.h"

#include <vector>

#include <string.h>
#include <random>

#ifdef _MSC_VER
#else
#include <sys/stat.h>
#endif

//#define CPPZ_OUT_LOG
//#define CPPZ_TEST_VERY_LARGE

using namespace cppimg;

int def(u8* dst, u32 srcSize, u8* src)
{
    int ret, flush;
    mz_stream stream;
    stream.zalloc = CPPIMG_NULL;
    stream.zfree = CPPIMG_NULL;
    stream.opaque = CPPIMG_NULL;
    ret = mz_deflateInit(&stream, 6);
    if(MZ_OK != ret){
        return ret;
    }

    const int Chunk = 16384;
    u8 in[Chunk];
    u8 out[Chunk];
    u32 count = 0;
    int outCount = 0;
    do{
        if(srcSize<=count){
            mz_deflateEnd(&stream);
            return outCount;
        }
        u32 size = Chunk;
        if(srcSize<(count+size)){
            size = srcSize-count;
        }
        stream.avail_in = size;
        memcpy(in, src+count, size);
        count += size;
        flush = (srcSize<=count)? MZ_FINISH : MZ_NO_FLUSH;
        stream.next_in = in;
        do{
            stream.avail_out = Chunk;
            stream.next_out = out;
            ret = mz_deflate(&stream, flush);
            int s = Chunk - stream.avail_out;
            memcpy(dst+outCount, out, s);
            outCount += s;
        }while(stream.avail_out == 0);
    }while(flush != MZ_FINISH);
    mz_deflateEnd(&stream);
    return outCount;
}

int inf(u8* dst, u32 srcSize, u8* src)
{
    int ret;
    mz_stream stream;
    stream.zalloc = CPPIMG_NULL;
    stream.zfree = CPPIMG_NULL;
    stream.opaque = CPPIMG_NULL;
    ret = mz_inflateInit(&stream);
    if(MZ_OK != ret){
        return ret;
    }

    const int Chunk = 16384;
    u8 in[Chunk];
    u8 out[Chunk];
    u32 count = 0;
    int outCount = 0;
    do{
        if(srcSize<=count){
            mz_inflateEnd(&stream);
            return MZ_OK;
        }
        u32 size = Chunk;
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
            ret = mz_inflate(&stream, MZ_NO_FLUSH);
            switch(ret)
            {
            case MZ_NEED_DICT:
                ret = MZ_DATA_ERROR;
                break;
            case MZ_DATA_ERROR:
            case MZ_MEM_ERROR:
                mz_inflateEnd(&stream);
                return ret;
            }
            int s = Chunk - stream.avail_out;
            memcpy(dst+outCount, out, s);
            outCount += s;
        } while(stream.avail_out == 0);
    } while(ret != MZ_STREAM_END);
    mz_inflateEnd(&stream);
    return ret == MZ_STREAM_END? outCount : MZ_DATA_ERROR;
}

void test(int loopCount, u32 size)
{
    //size = 1048576;
    u32 seed_;
    u32 length;
    std::mt19937 mt;
    std::random_device rand;
    const u32 MaxSource = size;
    u8* compressed0 = reinterpret_cast<u8*>(malloc(MaxSource*3));
    u8* decompressed0 = reinterpret_cast<u8*>(malloc(MaxSource));

    u8* compressed1 = reinterpret_cast<u8*>(malloc(MaxSource*3));
    u8* decompressed1 = reinterpret_cast<u8*>(malloc(MaxSource));
    u8* source = reinterpret_cast<u8*>(malloc(MaxSource));

    std::uniform_int_distribution<u32> dist(MaxSource/2, MaxSource);
    std::uniform_int_distribution<u32> dist_byte(0, 255);
    for(int i=0; i<loopCount; ++i){
        seed_ = rand();
        mt.seed(seed_);
#ifdef CPPZ_OUT_LOG
        printf("seed %u\n", seed_);
#endif

        length = dist(mt);
        for(u32 j=0; j<length; ++j){
            source[j] = static_cast<u8>(dist_byte(mt));
        }

#ifdef CPPZ_OUT_LOG
        printf("source length: %d\n", length);
#endif
        u32 count;
#ifdef CPPZ_TEST_ZLIB
        int ret0_0 = def(compressed0, length, source);
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

        int ret1_0 = def(compressed1, length, source);
        int ret1_1 = inf(decompressed1, ret1_0, compressed1);
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
        REQUIRE(static_cast<u32>(ret1_1) == length);
        REQUIRE(count == length);
    }

    free(source);
    free(decompressed1);
    free(compressed1);
    free(decompressed0);
    free(compressed0);
}



void test(const char* path)
{
    FILE* file = CPPIMG_FOPEN(path, "rb");
    if(CPPIMG_NULL == file){
        return;
    }
#ifdef _MSC_VER
    s32 fd = _fileno(file);
    struct _stat fs;
    _fstat(fd, &fs);
#else
    s32 fd = fileno(file);
    struct stat fs;
    fstat(fd, &fs);
#endif
    u32 size = fs.st_size;

    u8* compressed0 = reinterpret_cast<u8*>(malloc(size*3));
    u8* decompressed0 = reinterpret_cast<u8*>(malloc(size));

    u8* compressed1 = reinterpret_cast<u8*>(malloc(size*3));
    u8* decompressed1 = reinterpret_cast<u8*>(malloc(size));
    u8* source = reinterpret_cast<u8*>(malloc(size));

    fread(source, size, 1, file);
    fclose(file);

    u32 count;

    int ret1_0 = def(compressed1, size, source);
    int ret1_1 = inf(decompressed1, ret1_0, compressed1);

    for(count=0; count<size; ++count){
        if(decompressed1[count] != source[count]){
            break;
        }
    }

    REQUIRE(static_cast<u32>(ret1_1) == size);
    REQUIRE(count == size);

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


