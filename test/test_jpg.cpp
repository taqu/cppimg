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
        cppimg::IFStream file;
        cppimg::s32 width, height;
        cppimg::ColorType colorType;
        char buffer[128];
        SPRINTF(buffer, "%s%s", directory, src);
        if(!file.open(buffer)){
            CHECK(false);
            return;
        }

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
        test("test00.jpg", "test00.jpg.bmp", "../data/");
    }

    SECTION("test01.jpg"){
        test("test01.jpg", "test01.jpg.bmp", "../data/");
    }

    SECTION("test02.jpg"){
        test("test02.jpg", "test02.jpg.bmp", "../data/");
    }
    SECTION("lena.jpg"){
        test("lena.jpg", "lena.jpg.bmp", "../data/");
    }
}
