#ifndef INC_CPPIMG_H_
#define INC_CPPIMG_H_
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/
/**
@file cppimg.h

USAGE:
put '#define CPPIMG_IMPLEMENTATION' before including this file to create the implementation.

*/
#include <cassert>
#include <cstdint>
#include <utility>

//#define CPPIMG_IMPLEMENTATION

#include "miniz/miniz.h"

#ifdef CPPIMG_IMPLEMENTATION
#include "miniz/miniz.c"
#include "miniz/miniz_tdef.c"
#include "miniz/miniz_tinfl.c"
#endif

namespace cppimg
{
#ifdef __cplusplus
#   if 201103L<=__cplusplus || 1900<=_MSC_VER
#       define CPPIMG_CPP11 1
#   endif
#endif

#ifdef __cplusplus
#   ifdef CPPIMG_CPP11
#       define CPPIMG_NULL nullptr
#   else
#       define CPPIMG_NULL 0
#   endif
#else
#   define CPPIMG_NULL (void*)0
#endif

#ifndef CPPIMG_TYPES
#define CPPIMG_TYPES
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

    typedef ::size_t size_t;
#endif

    using std::move;

#ifdef _MSC_VER
#ifndef CPPIMG_OFF_T
#define CPPIMG_OFF_T
    typedef s32 off_t;
#endif

#ifndef CPPIMG_FSEEK
#define CPPIMG_FSEEK(f,p,o) fseek((f),(p),(o))
#endif

#ifndef CPPIMG_FTELL
#define CPPIMG_FTELL(f) ftell((f))
#endif

#ifndef CPPIMG_SPRINTF
#define CPPIMG_SPRINTF(b,f, ...) sprintf_s(b, f, __VA_ARGS__)
#endif

    inline FILE* CPPIMG_FOPEN(const Char* filepath, const Char* mode)
    {
        FILE* file = CPPIMG_NULL;
        return 0 != ::fopen_s(&file, filepath, mode) ? file : CPPIMG_NULL;
    }
    
#else
#ifndef CPPIMG_OFF_T
#define CPPIMG_OFF_T
    typedef s32 off_t;
#endif

#ifndef CPPIMG_FSEEK
#define CPPIMG_FSEEK(f,p,o) fseek((f),(p),(o))
#endif

#ifndef CPPIMG_FTELL
#define CPPIMG_FTELL(f) ftell((f))
#endif

#ifndef CPPIMG_SPRINTF
#define CPPIMG_SPRINTF(b,f, ...) sprintf(b, f, __VA_ARGS__)
#endif

    inline FILE* CPPIMG_FOPEN(const Char* filepath, const Char* mode)
    {
        return ::fopen(filepath, mode);
    }
#endif

#ifndef CPPIMG_MALLOC
#define CPPIMG_MALLOC(size) malloc(size)
#endif

#ifndef CPPIMG_FREE
#define CPPIMG_FREE(ptr) free(ptr)
#endif

#ifndef CPPIMG_ASSERT
#define CPPIMG_ASSERT(exp) assert(exp)
#endif

    template<class T>
    inline T absolute(T x)
    {
        return 0<=x? x : -x;
    }

    template<class T>
    inline void swap(T& x0, T& x1)
    {
        T t = cppimg::move(x0);
        x0 = cppimg::move(x1);
        x1 = cppimg::move(t);
    }

    static const u32 MaxWidth = 0x7FFFFFFFU;
    static const u32 MaxHeight = 0x7FFFFFFFU;
    enum ColorType
    {
        ColorType_GRAY,
        ColorType_RGB,
        ColorType_RGBA,
    };
    inline s32 getBytesPerPixel(ColorType colorType)
    {
        switch(colorType)
        {
        case ColorType_GRAY:
            return 1;
        case ColorType_RGB:
            return 3;
        case ColorType_RGBA:
        default:
            return 4;
        };
    }

    //----------------------------------------------------
    //---
    //--- Stream
    //---
    //----------------------------------------------------
    class Stream
    {
    public:
        virtual bool valid() const =0;
        virtual bool seek(off_t pos, s32 whence) =0;
        virtual off_t tell() =0;
    protected:
        Stream(){}
        ~Stream(){}
    private:
        Stream(const Stream&) = delete;
        Stream& operator=(const Stream&) = delete;
    };
    //----------------------------------------------------
    //---
    //--- IStream
    //---
    //----------------------------------------------------
    class IStream : public Stream
    {
    public:
        virtual s32 read(size_t size, void* dst) =0;
    protected:
        IStream(){}
        ~IStream(){}
    private:
        IStream(const IStream&) = delete;
        IStream& operator=(const IStream&) = delete;
    };

    //----------------------------------------------------
    //---
    //--- OStream
    //---
    //----------------------------------------------------
    class OStream : public Stream
    {
    public:
        virtual s32 write(size_t size, const void* dst) =0;
    protected:
        OStream(){}
        ~OStream(){}
    private:
        OStream(const OStream&) = delete;
        OStream& operator=(const OStream&) = delete;
    };

    //----------------------------------------------------
    //---
    //--- FStream
    //---
    //----------------------------------------------------
    template<class T>
    class FStream : public T
    {
    public:
        void close();

        virtual bool valid() const;
        virtual bool seek(off_t pos, s32 whence);
        virtual off_t tell();

    protected:
        FStream(const FStream&) = delete;
        FStream& operator=(const FStream&) = delete;

        bool open(const Char* filepath, const Char* mode);

        FStream();
        FStream(FStream&& rhs);
        ~FStream();

        FILE* file_;
    };

    template<class T>
    FStream<T>::FStream()
        :file_(CPPIMG_NULL)
    {}

    template<class T>
    FStream<T>::FStream(FStream&& rhs)
        :file_(rhs.file_)
    {
        rhs.file_ = CPPIMG_NULL;
    }

    template<class T>
    FStream<T>::~FStream()
    {
        close();
    }

    template<class T>
    bool FStream<T>::open(const Char* filepath, const Char* mode)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != filepath);
        CPPIMG_ASSERT(CPPIMG_NULL != mode);
        FILE* file = CPPIMG_FOPEN(filepath, mode);
        if(CPPIMG_NULL == file){
            return false;
        }
        close();
        file_ = file;
        return true;
    }

    template<class T>
    void FStream<T>::close()
    {
        if(CPPIMG_NULL != file_){
            fclose(file_);
            file_ = CPPIMG_NULL;
        }
    }

    template<class T>
    bool FStream<T>::valid() const
    {
        return CPPIMG_NULL != file_;
    }

    template<class T>
    bool FStream<T>::seek(off_t pos, s32 whence)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        return 0<=CPPIMG_FSEEK(file_, pos, whence);
    }

    template<class T>
    off_t FStream<T>::tell()
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        return CPPIMG_FTELL(file_);
    }

    //----------------------------------------------------
    //---
    //--- IFStream
    //---
    //----------------------------------------------------
    class IFStream : public FStream<IStream>
    {
    public:
        typedef FStream<IStream> parent_type;

        IFStream()
        {}
        IFStream(IFStream&& rhs)
            :parent_type(cppimg::move(rhs))
        {}
        ~IFStream()
        {
            close();
        }

        bool open(const Char* filepath)
        {
            return parent_type::open(filepath, "rb");
        }

        virtual s32 read(size_t size, void* dst);

        IFStream& operator=(IFStream&& rhs);
    private:
        IFStream(const IFStream&) = delete;
        IFStream& operator=(const IFStream&) = delete;
    };

    //----------------------------------------------------
    //---
    //--- OFStream
    //---
    //----------------------------------------------------
    class OFStream : public FStream<OStream>
    {
    public:
        typedef FStream<OStream> parent_type;

        OFStream()
        {}
        OFStream(OFStream&& rhs)
            :parent_type(cppimg::move(rhs))
        {}
        ~OFStream()
        {
            close();
        }

        bool open(const Char* filepath)
        {
            return parent_type::open(filepath, "wb");
        }

        virtual s32 write(size_t size, const void* dst);

        OFStream& operator=(OFStream&& rhs);
    private:
        OFStream(const OFStream&) = delete;
        OFStream& operator=(const OFStream&) = delete;
    };

    //----------------------------------------------------
    //---
    //--- BMP
    //---
    //----------------------------------------------------
    class BMP
    {
    public:
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool read(s32& width, s32& height, ColorType& colorType, void* image, IStream& stream);

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image);

    private:
        static const u16 MAGIC = 0x4D42U;//'MB';

        static const u32 Compression_RGB = 0;
        static const u32 Compression_RLE8 = 1;
        static const u32 Compression_RLE4 = 2;
        static const u32 Compression_BitFields = 3;
        static const u32 Compression_JPEG = 4;
        static const u32 Compression_PNG = 5;

        typedef u32 FXPT2DOT30;
        struct CIEXYZ
        {
            FXPT2DOT30 x_;
            FXPT2DOT30 y_;
            FXPT2DOT30 z_;
        };

        struct CIEXYZTRIPLE
        {
            CIEXYZ r_;
            CIEXYZ g_;
            CIEXYZ b_;
        };

        struct HEADER
        {
            //u16 type_; /// magic number
            u32 size_; /// file size
            u16 reserve1_;
            u16 reserve2_;
            u32 offset_; /// offset from file head to data

        };

        struct INFOHEADER
        {
            u32 infoSize_;
            s32 width_;
            s32 height_;
            u16 planes_;
            u16 bitCount_;
            u32 compression_;
            u32 sizeImage_;
            s32 xPixPerMeter_;
            s32 yPixPerMeter_;
            u32 colorUsed_;
            u32 colorImportant_;
        };

        struct INFOHEADERV4 : public INFOHEADER
        {
            u32 maskR_;
            u32 maskG_;
            u32 maskB_;
            u32 maskA_;
            u32 colorSpace_;
            CIEXYZTRIPLE endpoints_;
            u32 gammaR_;
            u32 gammaG_;
            u32 gammaB_;
        };

        struct INFOHEADERV5 : public INFOHEADERV4
        {
            u32 intent_;
            u32 profileData_;
            u32 profileSize_;
            u32 reserved_;
        };

        static bool read24(u8* image, s32 width, s32 height, bool leftBottom, IStream& stream);
        static bool read32(u8* image, s32 width, s32 height, bool leftBottom, IStream& stream);
    };

    //----------------------------------------------------
    //---
    //--- TGA
    //---
    //----------------------------------------------------
    class TGA
    {
    public:
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool read(s32& width, s32& height, ColorType& colorType, void* image, IStream& stream);

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image);

    private:
        enum Type
        {
            Type_None = 0x00U,
            Type_IndexColor = 0x01U,
            Type_FullColor = 0x02U,
            Type_Gray = 0x03U,
            Type_IndexColorRLE = 0x09U,
            Type_FullColorRLE = 0x0AU,
            Type_GrayRLE = 0x0BU,
        };

        enum Offset
        {
            Offset_IDLeng = 0,
            Offset_ColorMapType,
            Offset_ImageType,

            Offset_ColorMapOriginL,
            Offset_ColorMapOriginH,

            Offset_ColorMapLengL,
            Offset_ColorMapLengH,

            Offset_ColorMapSize,

            Offset_OriginXL,
            Offset_OriginXH,
            Offset_OriginYL,
            Offset_OriginYH,

            Offset_WidthL,
            Offset_WidthH,
            Offset_HeightL,
            Offset_HeightH,
            Offset_BitPerPixel,
            Offset_Discripter,
        };
        static const u32 TGA_HEADER_SIZE = 18;

        static inline u16 getU16(u8 low, u8 high)
        {
            return (static_cast<u16>(high)<<8) + low;
        }
        static inline bool check(u8 bpp, const u8* x0, const u8* x1)
        {
            for(u8 i=0; i<bpp; ++i){
                if(x0[i] != x1[i]){
                    return false;
                }
            }
            return true;
        }

        static bool readInternal(s32 width, s32 height, u8 bpp, u8* image, IStream& stream);
        static bool readRLEInternal(s32 width, s32 height, u8 bpp, u8* image, IStream& stream);
        static u8 calcRunLength(u8 bpp, s32 x, s32 y, s32 width, const u8* image);
        //static bool write32(OStream& stream, s32 width, s32 height, const u8* image);
        static bool writeRLE(OStream& stream, s32 width, s32 height, u8 bpp, const u8* image);
    };

    //----------------------------------------------------
    //---
    //--- PPM
    //---
    //----------------------------------------------------
    class PPM
    {
    public:
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image);
    };

    //----------------------------------------------------
    //---
    //--- PNG
    //---
    //----------------------------------------------------
    class PNG
    {
    public:
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool read(s32& width, s32& height, ColorType& colorType, void* image, IStream& stream);

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image);
    private:
        static const u64 Signature = 0x0A1A0A0D474E5089U;

        static const u32 ColorType_Gray = 0;
        static const u32 ColorType_True = 2;
        static const u32 ColorType_Index = 3;
        static const u32 ColorType_GrayAlpha = 4;
        static const u32 ColorType_TrueAlpha = 6;

        static const u8 FilterType_None = 0;
        static const u8 FilterType_Sub = 1;
        static const u8 FilterType_Up = 2;
        static const u8 FilterType_Avg = 3;
        static const u8 FilterType_Paeth = 4;

        struct Chunk
        {
            u32 length_;
            u32 type_;
        };

        struct ChunkIHDR : public Chunk
        {
            static const u32 Type = 0x52444849U;//'RDHI';
            static const u32 Size = 13;
            bool read(IStream& stream);

            u32 width_;
            u32 height_;
            u8 bitDepth_;
            u8 colorType_;
            u8 compression_;
            u8 filter_;
            u8 interlace_;
        };

        struct ChunkPLTE : public Chunk
        {
            static const u32 Type = 0x45544C50U;//'ETLP';
            static const u32 MaxSize = 256;
            bool read(IStream& stream);

            u32 size_;
            u8 r_[MaxSize];
            u8 g_[MaxSize];
            u8 b_[MaxSize];
        };

        struct ChunkIDAT : public Chunk
        {
            static const u32 Type = 0x54414449U;//'TADI';
            static const u32 BufferSize = 1024;

            ChunkIDAT(bool deflate, u32 width, u32 height, s32 color, s32 alpha);
            ~ChunkIDAT();

            bool initialize();
            bool terminate();
            bool read(void* image, IStream& stream);
            bool write(OStream& stream, s32 height, const u8* image);
            void filter(u8* image);

            bool deflate_;
            mz_stream stream_;
            u32 totalSize_;
            u32 totalCount_;
            u32 color_;
            u32 alpha_;
            u32 width_;
            u32 scanlineSize_; //size in bytes
            u8* scanline_;
            u32 scanlineOffset_;
            s32 filterFlag_;
            u8 dst_[BufferSize];
        };

        struct ChunkIEND : public Chunk
        {
            static const u32 Type = 0x444E4549U;//'DNEI';
        };

        static inline u16 reverse(u16 x);
        static inline u32 reverse(u32 x);
        static bool readHeader(IStream& stream);
        static bool checkCRC32(const Chunk& chunk, IStream& stream);
        static bool skipChunk(const Chunk& chunk, IStream& stream);

        template<class T>
        static void setChunkHeader(T& dst, const Chunk& src);

        template<class T>
        static bool readHeader(T& chunk, IStream& stream);
        static bool writeChunk(OStream& stream, u32 type, u32 size, const void* data);
    };

}
#endif //INC_CPPIMG_H_

#ifdef CPPIMG_IMPLEMENTATION
namespace cppimg
{
namespace
{
    //----------------------------------------------------
    //--- CRC32
    //----------------------------------------------------
    u32 CRC32Table[] =
    {
        0x0U,0x77073096U,0xEE0E612CU,0x990951BAU,0x76DC419U,0x706AF48FU,0xE963A535U,0x9E6495A3U,0xEDB8832U,0x79DCB8A4U,0xE0D5E91EU,0x97D2D988U,0x9B64C2BU,0x7EB17CBDU,0xE7B82D07U,0x90BF1D91U,0x1DB71064U,0x6AB020F2U,0xF3B97148U,0x84BE41DEU,0x1ADAD47DU,0x6DDDE4EBU,0xF4D4B551U,0x83D385C7U,0x136C9856U,0x646BA8C0U,0xFD62F97AU,0x8A65C9ECU,0x14015C4FU,0x63066CD9U,0xFA0F3D63U,0x8D080DF5U,0x3B6E20C8U,0x4C69105EU,0xD56041E4U,0xA2677172U,0x3C03E4D1U,0x4B04D447U,0xD20D85FDU,0xA50AB56BU,0x35B5A8FAU,0x42B2986CU,0xDBBBC9D6U,0xACBCF940U,0x32D86CE3U,0x45DF5C75U,0xDCD60DCFU,0xABD13D59U,0x26D930ACU,0x51DE003AU,0xC8D75180U,0xBFD06116U,0x21B4F4B5U,0x56B3C423U,0xCFBA9599U,0xB8BDA50FU,0x2802B89EU,0x5F058808U,0xC60CD9B2U,0xB10BE924U,0x2F6F7C87U,0x58684C11U,0xC1611DABU,0xB6662D3DU,
        0x76DC4190U,0x1DB7106U,0x98D220BCU,0xEFD5102AU,0x71B18589U,0x6B6B51FU,0x9FBFE4A5U,0xE8B8D433U,0x7807C9A2U,0xF00F934U,0x9609A88EU,0xE10E9818U,0x7F6A0DBBU,0x86D3D2DU,0x91646C97U,0xE6635C01U,0x6B6B51F4U,0x1C6C6162U,0x856530D8U,0xF262004EU,0x6C0695EDU,0x1B01A57BU,0x8208F4C1U,0xF50FC457U,0x65B0D9C6U,0x12B7E950U,0x8BBEB8EAU,0xFCB9887CU,0x62DD1DDFU,0x15DA2D49U,0x8CD37CF3U,0xFBD44C65U,0x4DB26158U,0x3AB551CEU,0xA3BC0074U,0xD4BB30E2U,0x4ADFA541U,0x3DD895D7U,0xA4D1C46DU,0xD3D6F4FBU,0x4369E96AU,0x346ED9FCU,0xAD678846U,0xDA60B8D0U,0x44042D73U,0x33031DE5U,0xAA0A4C5FU,0xDD0D7CC9U,0x5005713CU,0x270241AAU,0xBE0B1010U,0xC90C2086U,0x5768B525U,0x206F85B3U,0xB966D409U,0xCE61E49FU,0x5EDEF90EU,0x29D9C998U,0xB0D09822U,0xC7D7A8B4U,0x59B33D17U,0x2EB40D81U,0xB7BD5C3BU,0xC0BA6CADU,
        0xEDB88320U,0x9ABFB3B6U,0x3B6E20CU,0x74B1D29AU,0xEAD54739U,0x9DD277AFU,0x4DB2615U,0x73DC1683U,0xE3630B12U,0x94643B84U,0xD6D6A3EU,0x7A6A5AA8U,0xE40ECF0BU,0x9309FF9DU,0xA00AE27U,0x7D079EB1U,0xF00F9344U,0x8708A3D2U,0x1E01F268U,0x6906C2FEU,0xF762575DU,0x806567CBU,0x196C3671U,0x6E6B06E7U,0xFED41B76U,0x89D32BE0U,0x10DA7A5AU,0x67DD4ACCU,0xF9B9DF6FU,0x8EBEEFF9U,0x17B7BE43U,0x60B08ED5U,0xD6D6A3E8U,0xA1D1937EU,0x38D8C2C4U,0x4FDFF252U,0xD1BB67F1U,0xA6BC5767U,0x3FB506DDU,0x48B2364BU,0xD80D2BDAU,0xAF0A1B4CU,0x36034AF6U,0x41047A60U,0xDF60EFC3U,0xA867DF55U,0x316E8EEFU,0x4669BE79U,0xCB61B38CU,0xBC66831AU,0x256FD2A0U,0x5268E236U,0xCC0C7795U,0xBB0B4703U,0x220216B9U,0x5505262FU,0xC5BA3BBEU,0xB2BD0B28U,0x2BB45A92U,0x5CB36A04U,0xC2D7FFA7U,0xB5D0CF31U,0x2CD99E8BU,0x5BDEAE1DU,
        0x9B64C2B0U,0xEC63F226U,0x756AA39CU,0x26D930AU,0x9C0906A9U,0xEB0E363FU,0x72076785U,0x5005713U,0x95BF4A82U,0xE2B87A14U,0x7BB12BAEU,0xCB61B38U,0x92D28E9BU,0xE5D5BE0DU,0x7CDCEFB7U,0xBDBDF21U,0x86D3D2D4U,0xF1D4E242U,0x68DDB3F8U,0x1FDA836EU,0x81BE16CDU,0xF6B9265BU,0x6FB077E1U,0x18B74777U,0x88085AE6U,0xFF0F6A70U,0x66063BCAU,0x11010B5CU,0x8F659EFFU,0xF862AE69U,0x616BFFD3U,0x166CCF45U,0xA00AE278U,0xD70DD2EEU,0x4E048354U,0x3903B3C2U,0xA7672661U,0xD06016F7U,0x4969474DU,0x3E6E77DBU,0xAED16A4AU,0xD9D65ADCU,0x40DF0B66U,0x37D83BF0U,0xA9BCAE53U,0xDEBB9EC5U,0x47B2CF7FU,0x30B5FFE9U,0xBDBDF21CU,0xCABAC28AU,0x53B39330U,0x24B4A3A6U,0xBAD03605U,0xCDD70693U,0x54DE5729U,0x23D967BFU,0xB3667A2EU,0xC4614AB8U,0x5D681B02U,0x2A6F2B94U,0xB40BBE37U,0xC30C8EA1U,0x5A05DF1BU,0x2D02EF8DU,
    };

    u32 updateCRC32(u32 crc, u32 len, const u8* buffer)
    {
        for(u32 i=0; i<len; ++i){
            crc = CRC32Table[(crc ^ buffer[i]) & 0xFFU] ^ (crc >> 8);
        }
        return crc;
    }

#if 0 //unused
    u32 CRC32(u32 len, const u8* buffer)
    {
        return updateCRC32(0xFFFFFFFFUL, len, buffer)^0xFFFFFFFFUL;
    }
#endif

    //----------------------------------------------------
    //--- SeekSet
    //----------------------------------------------------
    class SeekSet
    {
    public:
        SeekSet(off_t pos, Stream* stream)
            :pos_(pos)
            ,stream_(stream)
        {}
        ~SeekSet()
        {
            if(CPPIMG_NULL != stream_){
                stream_->seek(pos_, SEEK_SET);
            }
        }

        void clear()
        {
            stream_ = CPPIMG_NULL;
        }
    private:
        s32 pos_;
        Stream* stream_;
    };
}

    //----------------------------------------------------
    //---
    //--- IFStream
    //---
    //----------------------------------------------------
    s32 IFStream::read(size_t size, void* dst)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        return 0<fread(dst, size, 1, file_)? 1 : -1;
    }

    IFStream& IFStream::operator=(IFStream&& rhs)
    {
        if(this != &rhs){
            if(CPPIMG_NULL != file_){
                fclose(file_);
            }
            file_ = rhs.file_;
            rhs.file_ = CPPIMG_NULL;
        }
        return *this;
    }

    //----------------------------------------------------
    //---
    //--- OFStream
    //---
    //----------------------------------------------------
    s32 OFStream::write(size_t size, const void* dst)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        return 0<fwrite(dst, size, 1, file_)? 1 : -1;
    }

    OFStream& OFStream::operator=(OFStream&& rhs)
    {
        if(this != &rhs){
            if(CPPIMG_NULL != file_){
                fclose(file_);
            }
            file_ = rhs.file_;
            rhs.file_ = CPPIMG_NULL;
        }
        return *this;
    }

    //----------------------------------------------------
    //---
    //--- BMP
    //---
    //----------------------------------------------------
    bool BMP::read24(u8* image, s32 width, s32 height, bool leftBottom, IStream& stream)
    {
        s32 pitch = width * 3;
        s32 diff = (pitch + 0x03U) & (~0x03U);
        diff -= pitch;
        u8 tmp[3];

        if(leftBottom){
            image += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                u8 *b = image;
                for(s32 j = 0; j<width; ++j){
                    if(stream.read(3, tmp)<=0){
                        return false;
                    }
                    b[0] = tmp[2];
                    b[1] = tmp[1];
                    b[2] = tmp[0];
                    b += 3;
                }
                if(stream.read(diff, tmp)<=0){
                    return false;
                }
                image -= pitch;
            }

        }else{
            for(s32 i = 0; i<height; ++i){
                for(s32 j = 0; j<width; ++j){
                    if(stream.read(3, tmp)<=0){
                        return false;
                    }
                    image[0] = tmp[2];
                    image[1] = tmp[1];
                    image[2] = tmp[0];
                    image += 3;
                }
                if(stream.read(diff, tmp)<=0){
                    return false;
                }
            }
        }
        return true;
    }

    bool BMP::read32(u8* image, s32 width, s32 height, bool leftBottom, IStream& stream)
    {
        s32 pitch = width * 4;

        if(leftBottom){
            image += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                if(stream.read(pitch, image)<=0){
                    return false;
                }
                u8 *b = image;
                for(s32 j = 0; j<width; ++j){
                    cppimg::swap(b[0], b[3]);
                    cppimg::swap(b[1], b[2]);
                    b += 4;
                }
                image -= pitch;
            }

        } else{
            for(s32 i = 0; i<height; ++i){
                if(stream.read(pitch, image)<=0){
                    return false;
                }
                u8 *b = image;
                for(s32 j = 0; j<width; ++j){
                    cppimg::swap(b[0], b[3]);
                    cppimg::swap(b[1], b[2]);
                    b += 4;
                }
                image += pitch;
            }
        }
        return true;
    }

    bool BMP::read(s32& width, s32& height, ColorType& colorType, void* image, IStream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        u16 magic;
        if(stream.read(sizeof(u16), &magic)<=0){
            return false;
        }
        if(magic != MAGIC){
            return false;
        }

        HEADER header;
        if(stream.read(sizeof(HEADER), &header)<=0){
            return false;
        }

        INFOHEADERV5 infoHeader;
        if(stream.read(sizeof(u32), &infoHeader.infoSize_)<=0){
            return false;
        }
        switch(infoHeader.infoSize_)
        {
        case 40:
            if(stream.read(40-sizeof(u32), &infoHeader.width_)<=0){
                return false;
            }
            break;
        case 108:
            if(stream.read(108-sizeof(u32), &infoHeader.width_)<=0){
                return false;
            }
            break;
        case 124:
            if(stream.read(124-sizeof(u32), &infoHeader.width_)<=0){
                return false;
            }
            break;
        default:
            if(16<=infoHeader.infoSize_ && infoHeader.infoSize_<=64){
                if(stream.read(infoHeader.infoSize_-sizeof(u32), &infoHeader.width_)<=0){
                    return false;
                }
            }else{
                return false;
            }
            break;
        }

        //Support only rgb no-compression
        if(!(infoHeader.compression_ == Compression_RGB && 24 == infoHeader.bitCount_)
            && !(infoHeader.compression_ == Compression_BitFields && 32 == infoHeader.bitCount_)){
            return false;
        }

        //Support only 24 or 32 bits
        if(24 != infoHeader.bitCount_
            && 32 != infoHeader.bitCount_)
        {
            return false;
        }

        width = static_cast<u32>(infoHeader.width_);
        height = static_cast<u32>(infoHeader.height_);
        colorType = 24 == infoHeader.bitCount_? ColorType_RGB : ColorType_RGBA;
        if(CPPIMG_NULL == image){
            return true;
        }

        bool leftBottom = (0<=infoHeader.height_);
        switch(infoHeader.bitCount_){
        case 24:
            if(!read24(reinterpret_cast<u8*>(image), width, height, leftBottom, stream)){
                return false;
            }
            break;
        case 32:
            if(!read32(reinterpret_cast<u8*>(image), width, height, leftBottom, stream)){
                return false;
            }
            break;
        default:
            CPPIMG_ASSERT(false);
            break;
        }
        seekSet.clear();
        return true;
    }

    bool BMP::write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }
        SeekSet seekSet(stream.tell(), &stream);
        HEADER header = {0};
        INFOHEADER infoHeader = {0};
        u32 fileSize = sizeof(MAGIC);
        fileSize += sizeof(HEADER);
        fileSize += sizeof(INFOHEADER);

        header.reserve1_ = 0;
        header.reserve2_ = 0;
        header.size_ = sizeof(MAGIC)+sizeof(INFOHEADER);
        header.offset_ = fileSize;

        infoHeader.infoSize_ = 40;
        infoHeader.width_ = width;
        infoHeader.height_ = height;
        infoHeader.planes_ = 1;
        infoHeader.compression_ = Compression_RGB;
        infoHeader.sizeImage_ = 0;
        infoHeader.xPixPerMeter_ = 0;
        infoHeader.yPixPerMeter_ = 0;
        infoHeader.colorUsed_ = 0;
        infoHeader.colorImportant_ = 0;

        s32 pitch, dstPitch;
        switch(colorType)
        {
        case ColorType_GRAY:
        case ColorType_RGB:
        {
            infoHeader.bitCount_ = 24;
            pitch = 3*width;
            dstPitch = (pitch + 0x03U) & (~0x03U);
            infoHeader.sizeImage_ = dstPitch * height;
        }
        break;

        case ColorType_RGBA:
            infoHeader.bitCount_ = 32;
            pitch = dstPitch = width * 4;
            infoHeader.sizeImage_ = 4 * width * height;
            break;
        default:
            return false;
            break;
        };
        header.size_ += infoHeader.sizeImage_;
        u16 magic = MAGIC;
        if(stream.write(sizeof(magic), &magic)<=0){
            return false;
        }
        if(stream.write(sizeof(header), &header)<=0){
            return false;
        }
        if(stream.write(sizeof(infoHeader), &infoHeader)<=0){
            return false;
        }

        s32 diff = dstPitch - pitch;
        u8 tmp[4];
        const u8* buffer = reinterpret_cast<const u8*>(image);
        switch(colorType)
        {
        case ColorType_GRAY:
            buffer += width * (height-1);
            for(s32 i = 0; i<height; ++i){
                const u8* b = buffer;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = tmp[1] = tmp[2] = b[0];
                    if(stream.write(3, tmp)<=0){
                        return false;
                    }
                    ++b;
                }
                tmp[0] = tmp[1] = tmp[2] = 0;
                if(stream.write(diff, tmp)<=0){
                    return false;
                }
                buffer -= width;
            }
            break;
        case ColorType_RGB:
            buffer += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                const u8* b = buffer;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = b[2]; tmp[1] = b[1]; tmp[2] = b[0];
                    if(stream.write(3, tmp)<=0){
                        return false;
                    }
                    b += 3;
                }
                tmp[0] = tmp[1] = tmp[2] = 0;
                if(stream.write(diff, tmp)<=0){
                    return false;
                }
                buffer -= pitch;
            }
            break;
        case ColorType_RGBA:
            buffer += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                const u8* b = buffer;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = b[2];
                    tmp[1] = b[1];
                    tmp[2] = b[0];
                    tmp[3] = b[3];
                    if(stream.write(4, tmp)<=0){
                        return false;
                    }
                    b += 4;
                }
                buffer -= pitch;
            }
            break;
        default:
            break;
        };
        seekSet.clear();
        return true;
    }
    
    //----------------------------------------------------
    //---
    //--- TGA
    //---
    //----------------------------------------------------
    bool TGA::readInternal(s32 width, s32 height, u8 bpp, u8* image, IStream& stream)
    {
        //transpose
        s32 rowBytes = width*bpp;
        image += rowBytes*(height-1);

        u8 tmp[4];
        for(s32 i = 0; i<height; ++i){
            u8* row = image;
            for(s32 j = 0; j<width; ++j){
                if(stream.read(bpp, tmp)<=0){
                    return false;
                }
                row[0] = tmp[2];
                row[1] = tmp[1];
                row[2] = tmp[0];
                if(3<bpp){
                    row[3] = tmp[3];
                }
                row += bpp;
            }
            image -= rowBytes;
        }
        return true;
    }

    bool TGA::readRLEInternal(s32 width, s32 height, u8 bpp, u8* image, IStream& stream)
    {
        //transpose
        s32 rowBytes = width*bpp;
        image += rowBytes*(height-1);
        u8* row = image;
        s32 x=0;
        s32 pixels = width*height;
        u8 tmp[4];
        for(s32 i = 0; i<pixels;){
            u8 byte;
            if(stream.read(1, &byte)<=0){
                return false;
            }
            //If MSB is 1 then consecutive, else no-consecutive data
            s32 count = (byte & 0x7FU) + 1;
            if(0 != (byte & 0x80U)){
                //consecutive data
                if(stream.read(bpp, tmp)<=0){
                    return false;
                }
                for(s32 j = 0; j<count; ++j){
                    row[0] = tmp[2];
                    row[1] = tmp[1];
                    row[2] = tmp[0];
                    if(3<bpp){
                        row[3] = tmp[3];
                    }
                    row += bpp;
                    if(width<=++x){
                        x = 0;
                        image -= rowBytes;
                        row = image;
                    }
                }
            } else{
                //no-consecutive data
                for(s32 j = 0; j<count; ++j){
                    if(stream.read(bpp, tmp)<=0){
                        return false;
                    }
                    row[0] = tmp[2];
                    row[1] = tmp[1];
                    row[2] = tmp[0];
                    if(3<bpp){
                        row[3] = tmp[3];
                    }
                    row += bpp;
                    if(width<=++x){
                        x = 0;
                        image -= rowBytes;
                        row = image;
                    }
                }
            }
            i += count;
        }
        return true;
    }

    u8 TGA::calcRunLength(u8 bpp, s32 x, s32 y, s32 width, const u8* image)
    {
        static const u32 maxCount = 0x80U;
        if(0==y && x==(width-1)){
            return 0x80U;
        }
        u8 count = 1;
        s32 nx = x+1;
        s32 ny = (width<=nx)? y-1 : y;

        const u8* current = image+(x+y*width)*bpp;
        const u8* next = image+(nx+ny*width)*bpp;
        if(check(bpp, current, next)){
            while(0<=ny){
                x = nx;
                y = ny;
                current = next;
                if(width<=++nx){
                    if(--ny<0){
                        break;
                    }
                    nx = 0;
                }
                next = image+(nx+ny*width)*bpp;
                if(!check(bpp, current, next)){
                    break;
                }
                if(maxCount<=++count){
                    break;
                }
            }
            return 0x80U | (count-1);

        } else{
            while(0<=ny){
                x = nx;
                y = ny;
                current = next;
                if(width<=++nx){
                    if(--ny<0){
                        break;
                    }
                    nx = 0;
                }
                next = image+(nx+ny*width)*bpp;
                if(check(bpp, current, next)){
                    break;
                }
                if(maxCount<=++count){
                    break;
                }
            }
            return count-1;
        }
    }
#if 0
    bool TGA::write32(OStream& stream, s32 width, s32 height, const u8* image)
    {
        static const u32 bpp = 4;
        s32 pixels = width*height;
        u8 tmp[bpp];
        for(s32 i = 0; i<pixels; ++i){
            tmp[0] = image[2];
            tmp[1] = image[1];
            tmp[2] = image[0];
            tmp[3] = image[3];
            if(stream.write(bpp, tmp)<=0){
                return false;
            }
            image += bpp;
        }
        return true;
    }
#endif

    bool TGA::writeRLE(OStream& stream, s32 width, s32 height, u8 bpp, const u8* image)
    {
        s32 x=0,y=height-1;
        while(0<=y){
            u8 run = calcRunLength(bpp, x, y, width, image);
            u8 count = (run & 0x7FU)+1;

            if(stream.write(1, &run)<=0){
                return false;
            }

            if(0 != (run & 0x80U)){
                if(stream.write(bpp, image+(x+y*width)*bpp)<=0){
                    return false;
                }
                x += count;
                while(width<=x){
                    x -= width;
                    --y;
                }
            } else{
                for(u8 i=0; i<count; ++i){
                    const u8* dst = image+(x+y*width)*bpp;
                    if(stream.write(bpp, dst)<=0){
                        return false;
                    }
                    if(width<=++x){
                        x -= width;
                        --y;
                    }
                }
            }//if(0 != (run & 0x80U)
        }//while(0<=y)
        return true;
    }

    bool TGA::read(s32& width, s32& height, ColorType& colorType, void* image, IStream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        u8 tgaHeader[TGA_HEADER_SIZE];
        if(stream.read(TGA_HEADER_SIZE, tgaHeader)<=0){
            return false;
        }
        width = static_cast<s32>(getU16(tgaHeader[Offset_WidthL], tgaHeader[Offset_WidthH]));
        height = static_cast<s32>(getU16(tgaHeader[Offset_HeightL], tgaHeader[Offset_HeightH]));

        u8 bpp = tgaHeader[Offset_BitPerPixel];
        u8 type = tgaHeader[Offset_ImageType];

        //Support only full color
        if(Type_FullColor != type
            && Type_FullColorRLE != type)
        {
            return false;
        }

        //Support either 24 or 32 bit 
        if(24 != bpp
            && 32 != bpp)
        {
            return false;
        }

        colorType = (24 == bpp)? ColorType_RGB : ColorType_RGBA;
        if(CPPIMG_NULL == image){
            return true;
        }
        //Skip string, if it exists
        if(0<tgaHeader[Offset_IDLeng]){
            stream.seek(tgaHeader[Offset_IDLeng], SEEK_CUR);
        }
        //SKip color map, if it exists
        if(1<=tgaHeader[Offset_ColorMapType]){
            stream.seek(tgaHeader[Offset_IDLeng], SEEK_CUR);
            u32 length = static_cast<u32>(getU16(tgaHeader[Offset_ColorMapLengL], tgaHeader[Offset_ColorMapLengH]));
            length *= (tgaHeader[Offset_ColorMapSize] >> 3);
            stream.seek(length, SEEK_CUR);
        }

        switch(bpp)
        {
        case 24:
        {
            if(Type_FullColorRLE == type){
                readRLEInternal(width, height, 3, reinterpret_cast<u8*>(image), stream);
            } else{
                readInternal(width, height, 3, reinterpret_cast<u8*>(image), stream);
            }
        }
        break;
        case 32:
        {
            if(Type_FullColorRLE == type){
                readRLEInternal(width, height, 4, reinterpret_cast<u8*>(image), stream);
            } else{
                readInternal(width, height, 4, reinterpret_cast<u8*>(image), stream);
            }
        }
        break;
        default:
            return false;
        };
        seekSet.clear();
        return true;
    }

    bool TGA::write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }
        SeekSet seekSet(stream.tell(), &stream);
        u8 tgaHeader[TGA_HEADER_SIZE] = {0};

        tgaHeader[Offset_WidthL] = width & 0xFFU;
        tgaHeader[Offset_WidthH] = (width>>8) & 0xFFU;

        tgaHeader[Offset_HeightL] = height & 0xFFU;
        tgaHeader[Offset_HeightH] = (height>>8) & 0xFFU;

        tgaHeader[Offset_BitPerPixel] = (ColorType_RGB == colorType)? 24 : 32;

        //tgaHeader[Offset_ImageType] = Type_FullColor;
        tgaHeader[Offset_ImageType] = Type_FullColorRLE;

        tgaHeader[Offset_Discripter] = (ColorType_RGB == colorType)? 0 : 8; //bit depth of alpha channel

        if(stream.write(TGA_HEADER_SIZE, tgaHeader)<=0){
            return false;
        }
        //if(!write32RLE(stream, width, height, reinterpret_cast<const u8*>(image))){
        //    return false;
        //}
        switch(colorType){
        case ColorType_RGB:
            if(!writeRLE(stream, width, height, 3, reinterpret_cast<const u8*>(image))){
                return false;
            }
            break;
        case ColorType_RGBA:
            if(!writeRLE(stream, width, height, 4, reinterpret_cast<const u8*>(image))){
                return false;
            }
            break;
        default:
            return false;
        }
        seekSet.clear();
        return true;
    }

    //----------------------------------------------------
    //---
    //--- PPM
    //---
    //----------------------------------------------------
    bool PPM::write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }
        Char buffer[64];
        s32 scomponents, dcomponents;
        switch(colorType){
        case ColorType_GRAY:
            if(stream.write(3, "P2\n")<=0){
                return false;
            }
            scomponents = dcomponents = 1;
            break;
        case ColorType_RGB:
            if(stream.write(3, "P3\n")<=0){
                return false;
            }
            scomponents = dcomponents = 3;
            break;
        case ColorType_RGBA:
            if(stream.write(3, "P3\n")<=0){
                return false;
            }
            scomponents = 4;
            dcomponents = 3;
            break;
        default:
            return false;
        }
        s32 len;
        len = CPPIMG_SPRINTF(buffer, "%d %d\n255\n", width, height);
        if(stream.write(len, buffer)<=0){
            return false;
        }
        for(s32 i=0; i<height; ++i){
            if(0<i && stream.write(1, "\n")<=0){
                return false;
            }
            for(s32 j=0; j<width; ++j){
                s32 index = scomponents*(i*width+j);
                for(s32 k=0; k<dcomponents; ++k){
                    len = CPPIMG_SPRINTF(buffer, "%d ", reinterpret_cast<const u8*>(image)[index+k]);
                    if(stream.write(len, buffer)<=0){
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool PNG::read(s32& width, s32& height, ColorType& colorType, void* image, IStream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);
        if(!readHeader(stream)){
            return false;
        }

        ChunkIHDR chunkIHDR;
        if(!readHeader(chunkIHDR, stream)
            || !chunkIHDR.read(stream)
            || MaxWidth<chunkIHDR.width_
            || MaxHeight<chunkIHDR.height_){
            return false;
        }
        width = static_cast<s32>(chunkIHDR.width_);
        height = static_cast<s32>(chunkIHDR.height_);

        s32 color;
        s32 alpha = 0;
        switch(chunkIHDR.colorType_){
        case PNG::ColorType_Gray:
            colorType = ColorType_GRAY;
            color = 1;
            break;
        case PNG::ColorType_True:
            colorType = ColorType_RGB;
            color = 3;
            break;
        case PNG::ColorType_Index:
            colorType = ColorType_RGB;
            color = 1;
            break;
        case PNG::ColorType_GrayAlpha:
            colorType = ColorType_RGBA;
            color = 1;
            alpha = 1;
            break;
        case PNG::ColorType_TrueAlpha:
            colorType = ColorType_RGBA;
            color = 3;
            alpha = 1;
            break;
        default:
            return false;
        }
        if(CPPIMG_NULL == image){
            return true;
        }

        Chunk chunk;
        ChunkPLTE chunkPLTE;
        ChunkIDAT chunkIDAT(false, chunkIHDR.width_, chunkIHDR.height_, color, alpha);
        if(!chunkIDAT.initialize()){
            return false;
        }
        bool loop = true;
        do{
            if(!readHeader(chunk, stream)){
                return false;
            }
            //Support only critical chunks
            switch(chunk.type_)
            {
            case ChunkPLTE::Type:
                setChunkHeader(chunkPLTE, chunk);
                if(!chunkPLTE.read(stream)){
                    return false;
                }
                break;
            case ChunkIDAT::Type:
                setChunkHeader(chunkIDAT, chunk);
                if(!chunkIDAT.read(image, stream)){
                    return false;
                }
                break;
            case ChunkIEND::Type:
                loop = false;
                break;
            default:
                if(!skipChunk(chunk, stream)){
                    return false;
                }
                break;
            }
        }while(loop);

        chunkIDAT.terminate();

        u8* uimage = reinterpret_cast<u8*>(image);
        switch(chunkIHDR.colorType_){
        case PNG::ColorType_Index:
            for(s32 i=0; i<height; ++i){
                for(s32 j=0; j<width; ++j){
                    s32 index = (i*width+j)*3;
                    uimage[index+0] = chunkPLTE.r_[uimage[index+0]];
                    uimage[index+1] = chunkPLTE.g_[uimage[index+1]];
                    uimage[index+2] = chunkPLTE.b_[uimage[index+2]];
                }
            }
            break;
        }
        seekSet.clear();
        return true;
    }

    bool PNG::write(OStream& stream, s32 width, s32 height, ColorType colorType, const void* image)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        //Write signature
        u64 signature = Signature;
        if(stream.write(sizeof(u64), &signature)<0){
            return false;
        }

        //Write header
        ChunkIHDR chunkIHDR;
        chunkIHDR.width_ = reverse(static_cast<u32>(width));
        chunkIHDR.height_ = reverse(static_cast<u32>(height));
        chunkIHDR.bitDepth_ = 8;
        chunkIHDR.compression_ = 0;
        chunkIHDR.filter_ = 0;
        chunkIHDR.interlace_ = 0;

        s32 color;
        s32 alpha = 0;
        switch(colorType)
        {
        case ColorType_GRAY:
            chunkIHDR.colorType_ = PNG::ColorType_Gray;
            color = 1;
            break;
        case ColorType_RGB:
            chunkIHDR.colorType_ = PNG::ColorType_True;
            color = 3;
            break;
        case ColorType_RGBA:
            chunkIHDR.colorType_ = PNG::ColorType_TrueAlpha;
            color = 3;
            alpha = 1;
            break;
        default:
            return false;
        }
        if(!writeChunk(stream, ChunkIHDR::Type, ChunkIHDR::Size, &chunkIHDR.width_)){
            return false;
        }

        ChunkIDAT chunkIDAT(true, width, height, color, alpha);
        if(!chunkIDAT.initialize()){
            return false;
        }
        if(!chunkIDAT.write(stream, height, reinterpret_cast<const u8*>(image))){
            return false;
        }
        chunkIDAT.terminate();
        if(!writeChunk(stream, ChunkIEND::Type, 0, CPPIMG_NULL)){
            return false;
        }
        seekSet.clear();
        return true;
    }

    //--- PNG::ChunkIHDR
    //----------------------------------------------------
    bool PNG::ChunkIHDR::read(IStream& istream)
    {
        if(istream.read(Size, &width_)<0){
            return false;
        }
        u32 crc;
        if(istream.read(sizeof(u32), &crc)<0){
            return false;
        }
        width_ = reverse(width_);
        height_ = reverse(height_);

        //Support only 8 bit depth
#if 0
        switch(color_){
        case PNG::ColorType_Gray:
            if(1!=bitDepth_
                && 2 != bitDepth_
                && 4 != bitDepth_
                && 8 != bitDepth_
                && 16 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_True:
            if(8 != bitDepth_ && 16 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_Index:
            if(1!=bitDepth_
                && 2 != bitDepth_
                && 4 != bitDepth_
                && 8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_GrayAlpha:
            if(8 != bitDepth_ && 16 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_TrueAlpha:
            if(8 != bitDepth_ && 16 != bitDepth_){
                return false;
            }
            break;
        default:
            return false;
        }
#else
        switch(colorType_){
        case PNG::ColorType_Gray:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_True:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_Index:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_GrayAlpha:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_TrueAlpha:
            if(8 != bitDepth_){
                return false;
            }
            break;
        default:
            return false;
        }
#endif

        //Don't support interlace
        if(0<interlace_){
            return false;
        }
        return true;
    }

    //--- PNG::ChunkPLTE
    //----------------------------------------------------
    bool PNG::ChunkPLTE::read(IStream& stream)
    {
        //It's assumed that (chunk size mod 3) equals zero
        if(0 != (length_%3)){
            return false;
        }

        u8 rgb[3];
        u32 length=length_/3;
        for(u32 i=0; i<length; ++i){
            if(stream.read(3, rgb)<0){
                return false;
            }
            r_[i] = rgb[0];
            g_[i] = rgb[1];
            b_[i] = rgb[2];
        }
        size_ = length;
        u32 crc;
        if(stream.read(sizeof(u32), &crc)<0){
            return false;
        }
        return true;
    }

    //--- ChunkIDAT
    //----------------------------------------------------
    PNG::ChunkIDAT::ChunkIDAT(bool deflate, u32 width, u32 height, s32 color, s32 alpha)
        :deflate_(deflate)
        ,totalSize_(width*height*(color+alpha))
        ,totalCount_(0)
        ,color_(color)
        ,alpha_(alpha)
        ,width_(width)
        ,scanlineSize_(width*(color+alpha))
        ,scanline_(CPPIMG_NULL)
        ,scanlineOffset_(0)
        ,filterFlag_(-1)
    {
        stream_.zalloc = CPPIMG_NULL;
        stream_.zfree = CPPIMG_NULL;
        stream_.opaque = CPPIMG_NULL;
        stream_.state = CPPIMG_NULL;
    }

    PNG::ChunkIDAT::~ChunkIDAT()
    {
        terminate();
    }

    bool PNG::ChunkIDAT::initialize()
    {
        if(CPPIMG_NULL == stream_.state){
            s32 result;
            if(deflate_){
                result = mz_deflateInit(&stream_, 6);
            }else{
                result = mz_inflateInit(&stream_);
            }
            stream_.avail_out = BufferSize;
            stream_.next_out = dst_;
            scanline_ = CPPIMG_NULL;
            return MZ_OK == result;
        }
        return false;
    }

    bool PNG::ChunkIDAT::terminate()
    {
        if(CPPIMG_NULL != stream_.state){
            s32 result;
            if(deflate_){
                result = mz_deflateEnd(&stream_);
            }else{
                result = mz_inflateEnd(&stream_);
            }
            stream_.state = CPPIMG_NULL;
            return MZ_OK == result;
        }
        return true;
    }


    bool PNG::ChunkIDAT::read(void* image, IStream& stream)
    {
        u8 src_[BufferSize];

        u32 chunkCount = 0;
        s32 result;
        if(CPPIMG_NULL == scanline_){
            scanline_ = reinterpret_cast<u8*>(image);
        }
        stream_.avail_in = 0;
        u8* dst = stream_.next_out;
        do{
            if(stream_.avail_out<=0){
                stream_.avail_out = BufferSize;
                stream_.next_out = dst_;
                dst = dst_;
            }
            if(stream_.avail_in<=0){
                stream_.next_in = src_;
                u32 size = BufferSize;
                if(length_<(chunkCount+size)){
                    size = length_-chunkCount;
                }
                if(size<=0){
                    break;
                }
                stream_.avail_in = size;
                if(stream.read(size, src_)<0){
                    return false;
                }
                chunkCount += size;
            }
            u64 totalOut = stream_.total_out;
            result = mz_inflate(&stream_, MZ_NO_FLUSH);
            if(MZ_OK != result && MZ_STREAM_END != result){
                return false;
            }
            u32 outSize = static_cast<u32>(stream_.total_out - totalOut);

            //Copy inflated data to scanline
            while(0<outSize && totalCount_<totalSize_){
                if(scanlineOffset_<=0){
                    --outSize;
                    filterFlag_ = dst[0];
                    ++dst;
                }

                u32 copySize = ((scanlineOffset_+outSize) < scanlineSize_)? outSize : scanlineSize_-scanlineOffset_;
                memcpy(scanline_+scanlineOffset_, dst, copySize);
                scanlineOffset_ += copySize;

                //Scanline have been filled up, apply filter
                if(scanlineSize_<=scanlineOffset_){
                    CPPIMG_ASSERT(scanlineSize_ == scanlineOffset_);
                    filter(reinterpret_cast<u8*>(image));
                    filterFlag_ = -1;
                    scanline_ += scanlineSize_;
                    scanlineOffset_ = 0;
                }
                totalCount_ += copySize;
                outSize -= copySize;
                dst += copySize;
            }
        }while(MZ_STREAM_END != result && totalCount_<totalSize_);

        u32 crc;
        if(stream.read(sizeof(u32), &crc)<0){
            return false;
        }
        return true;
    }

    bool PNG::ChunkIDAT::write(OStream& stream, s32 height, const u8* image)
    {
        struct ReleaseMemory
        {
            ReleaseMemory(u8* ptr)
                :ptr_(ptr)
            {}
            ~ReleaseMemory()
            {
                CPPIMG_FREE(ptr_);
            }
            u8* ptr_;
        };

        s32 lenPos = stream.tell();
        u32 outCount = 0;
        if(stream.write(sizeof(u32), &outCount)<=0){
            return false;
        }
        u32 type = Type;
        if(stream.write(sizeof(u32), &type)<=0){
            return false;
        }
        u32 crc = 0xFFFFFFFFUL;
        crc = updateCRC32(crc, sizeof(u32), reinterpret_cast<const u8*>(&type));

        //Add 1 byte of filter type for each scanlines
        //---------------------------------
        u32 totalSize = totalSize_ + height;

        u8* in = reinterpret_cast<u8*>(CPPIMG_MALLOC(totalSize));
        ReleaseMemory releaseInMemory(in);

        {
            u8 filter = 0;
            const u8* src = image;
            u8* dst = in;
            for(s32 i=0; i<height; ++i){
                dst[0] = filter;
                ++dst;
                memcpy(dst, src, scanlineSize_);
                src += scanlineSize_;
                dst += scanlineSize_;
            }
        }

        //---------------------------------
        u32 inCount = 0;
        s32 result;
        s32 flush;
        do{
            if(totalSize<=inCount){
                break;
            }
            u32 size = totalSize;
            if(totalSize<(inCount+size)){
                size = totalSize-inCount;
            }
            stream_.avail_in = size;
            stream_.next_in = in+inCount;
            inCount += size;
            flush = (totalSize<=inCount)? MZ_FINISH : MZ_NO_FLUSH;
            do{
                stream_.avail_out = BufferSize;
                stream_.next_out = dst_;
                u64 totalOut = stream_.total_out;
                result = mz_deflate(&stream_, flush);
                if(MZ_OK != result && MZ_STREAM_END != result){
                    return false;
                }
                u32 outSize = static_cast<u32>(stream_.total_out - totalOut);
                if(0<outSize){
                    crc = updateCRC32(crc, outSize, dst_);
                    if(stream.write(outSize, dst_)<=0){
                        return false;
                    }
                }
                outCount += outSize;
            }while(MZ_STREAM_END != result);
        }while(flush != MZ_FINISH);

        s32 crcPos = stream.tell();
        stream.seek(lenPos, SEEK_SET);
        outCount = reverse(outCount);
        if(stream.write(sizeof(u32), &outCount)<=0){
            return false;
        }
        stream.seek(crcPos, SEEK_SET);
        crc ^= 0xFFFFFFFFUL;
        crc = reverse(crc);
        return stream.write(sizeof(u32), &crc)<=0;
    }

    void PNG::ChunkIDAT::filter(u8* image)
    {
        CPPIMG_ASSERT(image<=scanline_ && scanline_<(image+totalSize_));
        u32 components = color_+alpha_;
        switch(filterFlag_)
        {
        case FilterType_Sub:
            for(u32 i=1; i<width_; ++i){
                u32 p = i*components;
                for(u32 j = 0; j<components; ++j, ++p){
                    s32 x = scanline_[p] + scanline_[p-components];
                    scanline_[p] = static_cast<u8>(x&255);
                }
            }
            break;
        case FilterType_Up:
            if(image<scanline_){
                u8* upper = scanline_-scanlineSize_;
                for(u32 i=0; i<width_; ++i){
                    u32 p = i*components;
                    for(u32 j = 0; j<components; ++j, ++p){
                        s32 x = upper[p] + scanline_[p];
                        scanline_[p] = static_cast<u8>(x&255);
                    }
                }
            }
            break;
        case FilterType_Avg:
            if(image<scanline_){
                u8* upper = scanline_-scanlineSize_;
                for(u32 i=1; i<width_; ++i){
                    u32 p = i*components;
                    for(u32 j = 0; j<components; ++j, ++p){
                        s32 x = ((scanline_[p-components] + upper[p])>>1) + scanline_[p];
                        scanline_[p] = static_cast<u8>(x&255);
                    }
                }
            }else{
                for(u32 i=1; i<width_; ++i){
                    u32 p = i*components;
                    for(u32 j = 0; j<components; ++j, ++p){
                        s32 x = (scanline_[p-components]>>1) + scanline_[p];
                        scanline_[p] = static_cast<u8>(x&255);
                    }
                }
            }
            break;
        case FilterType_Paeth:
        {
            u8* upper = scanline_-scanlineSize_;
            for(u32 i=0; i<width_; ++i){
                u32 p = i*components;
                for(u32 j = 0; j<components; ++j, ++p){
                    s32 a = (0<i)? scanline_[p-components] : 0;
                    s32 b = (image<scanline_)? upper[p] : 0;
                    s32 c = (0<i && image<scanline_)? upper[p-components] : 0;
                    s32 x = a+b-c;
                    s32 pa = absolute(x-a);
                    s32 pb = absolute(x-b);
                    s32 pc = absolute(x-c);
                    if(pa<=pb && pa<=pc){
                        x = a;
                    } else{
                        x = (pb<=pc)? b : c;
                    }
                    x += scanline_[p];
                    scanline_[p] = static_cast<u8>(x&255);
                }
            }
        }
        break;
        default:
            break;
        }
    }

    //--- PNG
    //----------------------------------------------------
    inline u16 PNG::reverse(u16 x)
    {
        return ((x>>8)&0xFFU) | ((x<<8)&0xFF00U);
    }

    inline u32 PNG::reverse(u32 x)
    {
        return ((x>>24)) | ((x>>8)&0xFF00U) | ((x&0xFF00U)<<8) | ((x&0xFFU)<<24);
    }

    bool PNG::readHeader(IStream& stream)
    {
        u64 signature;
        if(stream.read(sizeof(u64), &signature)<0){
            return false;
        }
        if(Signature != signature){
            return false;
        }
        return true;
    }

    bool PNG::checkCRC32(const Chunk& chunk, IStream& stream)
    {
        static const u32 BufferSize = 1024;
        u8 buffer[BufferSize];

        u32 crc = 0xFFFFFFFFUL;
        crc = updateCRC32(crc, sizeof(u32), reinterpret_cast<const u8*>(&chunk.type_));
        off_t pos = stream.tell();
        u32 remain=chunk.length_;
        while(0<remain){
            u32 size = (remain<BufferSize)? remain : BufferSize;
            if(stream.read(size, buffer)<0){
                break;
            }
            crc = updateCRC32(crc, size, buffer);
            remain -= size;
        }

        //Check whether calculated crc equals loaded crc
        u32 readCrc;
        s32 ret = (remain<=0)? stream.read(sizeof(u32), &readCrc) : 0;
        stream.seek(pos, SEEK_SET);
        if(ret<0){
            return false;
        }
        crc ^= 0xFFFFFFFFUL;
        readCrc = reverse(readCrc);
        return readCrc == crc;
    }

    bool PNG::skipChunk(const Chunk& chunk, IStream& stream)
    {
        u32 size = chunk.length_ + 4;//chunk data size + crc size
        return stream.seek(size, SEEK_CUR);
    }

    template<class T>
    void PNG::setChunkHeader(T& dst, const Chunk& src)
    {
        dst.type_ = src.type_;
        dst.length_ = src.length_;
    }

    template<class T>
    bool PNG::readHeader(T& chunk, IStream& stream)
    {
        if(stream.read(sizeof(u32)*2, &chunk)<0){
            return false;
        }
        chunk.length_ = reverse(chunk.length_);
        if(!checkCRC32(chunk, stream)){
            return false;
        }
        return true;
    }

    bool PNG::writeChunk(OStream& stream, u32 type, u32 size, const void* data)
    {
        u32 crc = 0xFFFFFFFFUL;
        crc = updateCRC32(crc, sizeof(u32), reinterpret_cast<const u8*>(&type));
        crc = updateCRC32(crc, size, reinterpret_cast<const u8*>(data));
        crc ^= 0xFFFFFFFFUL;

        u32 rcrc = reverse(crc);
        u32 rsize = reverse(size);

        if(stream.write(sizeof(u32), &rsize)<=0){
            return false;
        }
        if(stream.write(sizeof(u32), &type)<=0){
            return false;
        }
        if(0<size){
            if(stream.write(size, data)<=0){
                return false;
            }
        }
        if(stream.write(sizeof(u32), &rcrc)<=0){
            return false;
        }
        return true;
    }
}
#endif
