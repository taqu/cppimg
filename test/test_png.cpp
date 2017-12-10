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
        if(!cppimg::PNG::read(width, height, colorType, CPPIMG_NULL, file)){
            CHECK(false);
            return;
        }
        cppimg::u8* image = new cppimg::u8[width*height*cppimg::getBytesPerPixel(colorType)];
        if(cppimg::PNG::read(width, height, colorType, image, file)){
            cppimg::OFStream ofile;
            SPRINTF(buffer, "%s%s", directory, dst);
            if(ofile.open(buffer)){
                cppimg::PNG::write(ofile, width, height, colorType, image);
            }
        }
        delete[] image;
    }
}

TEST_CASE("Read/Write PNG" "[PNG]")
{
    SECTION("test00.png"){
        test("test00.png", "out00.png", "../data/");
    }
    SECTION("test01.png"){
        test("test01.png", "out01.png", "../data/");
    }
}
