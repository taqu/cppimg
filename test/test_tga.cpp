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
        if(!cppimg::TGA::read(width, height, colorType, CPPIMG_NULL, file)){
            CHECK(false);
            return;
        }
        cppimg::u8* image = new cppimg::u8[width*height*cppimg::getBytesPerPixel(colorType)];
        if(cppimg::TGA::read(width, height, colorType, image, file)){
            cppimg::OFStream ofile;
            SPRINTF(buffer, "%s%s", directory, dst);
            if(ofile.open(buffer)){
                cppimg::TGA::write(ofile, width, height, colorType, image);
            }
        }
        delete[] image;
    }
}

TEST_CASE("Read/Write TGA" "[PNG]")
{
    SECTION("test00.tga"){
        test("test00.tga", "out00_rle.tga", "../data/");
    }
    SECTION("test00_rle.tga"){
        test("test00_rle.tga", "out00_rle.tga", "../data/");
    }
    SECTION("test01.tga"){
        test("test01.tga", "out00_rle.tga", "../data/");
    }
    SECTION("test01_rle.tga"){
        test("test01_rle.tga", "out01_rle.tga", "../data/");
    }
}
