#include <stdio.h>

#include "catch.hpp"

#include "../cppimg.h"

namespace
{
#ifdef _WIN32
#define SPRINTF(BUFF, FORMAT, VAR0, VAR1) sprintf_s((BUFF), (FORMAT), (VAR0), (VAR1))
#else
#define SPRINTF(BUFF, FORMAT, VAR0, VAR1) sprintf((BUFF), (FORMAT), (VAR0), (VAR1))
#endif
    void test(const char* src, const char* dst, const char* directory)
    {
        cppimg::s32 f14020 = cppimg::fixed32_construct(1.4020f, cppimg::JPEG::FIXED_POINT_SHIFT);
        cppimg::s32 f03441 = cppimg::fixed32_construct(0.3441f, cppimg::JPEG::FIXED_POINT_SHIFT);
        cppimg::s32 f07139 = cppimg::fixed32_construct(0.7139f, cppimg::JPEG::FIXED_POINT_SHIFT);
        cppimg::s32 f17718 = cppimg::fixed32_construct(1.7718f, cppimg::JPEG::FIXED_POINT_SHIFT);

        printf("1.4020f = %X\n", f14020);
        printf("0.3441f = %X\n", f03441);
        printf("0.7139f = %X\n", f07139);
        printf("1.7718f = %X\n", f17718);

        cppimg::u8 huff_code[0xFFFF] = {0};
        cppimg::u8 huff_bits[0xFFFF] = {0};
        {
            int huff_size[] ={
                2, //1
                2, //2
                3, //3
                4, 4, 4, 5, 5, 5, 6, 6, 7, 7, 7, 7, 8,
            };
            int code =0;
            int si = huff_size[0];
            int k=0;
            for(;;){
                while(huff_size[k] == si){
                    huff_code[k] = code;
                    ++code;
                    ++k;
                }
                if(huff_size[k] <= 0){
                    break;
                }
                do{
                    code <<= 1;
                    si += 1;
                }while(huff_size[k] != si);
            }
        }

        cppimg::IFStream file;
        cppimg::s32 width, height;
        cppimg::ColorType colorType;
        char buffer[128];
        SPRINTF(buffer, "%s%s", directory, src);
        if(!file.open(buffer)){
            CHECK(false);
            return;
        }
        if(!cppimg::JPEG::read(width, height, colorType, CPPIMG_NULL, file)){
            CHECK(false);
            return;
        }
        cppimg::u8* image = new cppimg::u8[width*height*cppimg::getBytesPerPixel(colorType)];
        if(cppimg::JPEG::read(width, height, colorType, image, file)){
            cppimg::OFStream ofile;
            SPRINTF(buffer, "%s%s", directory, dst);
            if(ofile.open(buffer)){
                cppimg::BMP::write(ofile, width, height, colorType, image);
            }
        }else{
            CHECK(false);
        }
        delete[] image;
    }
}

TEST_CASE("Read JPG" "[JPG]")
{
    SECTION("test00.jpg"){
        test("testimg.jpg", "out00.jpg.bmp", "../data/");
    }
}
