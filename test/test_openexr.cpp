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
    void load(const char* src, const char* dst, const char* directory)
    {
        cppimg::IFStream file;
        char buffer[128];
        SPRINTF(buffer, "%s%s", directory, src);
        if(!file.open(buffer)){
            CHECK(false);
            return;
        }

        cppimg::OpenEXR::Information information;
        if(!cppimg::OpenEXR::read(information, CPPIMG_NULL, file)){
            CHECK(false);
            return;
        }

        cppimg::u8* image = new cppimg::u8[information.width_*information.height_*information.getBytesPerPixel()];
        if(cppimg::OpenEXR::read(information, image, file)){
            file.close();
            cppimg::u8* image8 = new cppimg::u8[information.width_*information.height_*information.numChannels_];
            cppimg::convert(information.width_, information.height_, information.numChannels_, image8, image, information.types_);
            cppimg::OFStream ofile;
            SPRINTF(buffer, "%s%s", directory, dst);
            if(ofile.open(buffer)){
                cppimg::BMP::write(ofile, information.width_, information.height_, information.colorType_, image8);
            }
            delete[] image8;
        }else{
            CHECK(false);
        }
        delete[] image;
    }

    void save(const char* src, const char* dst, const char* directory)
    {
        cppimg::IFStream file;
        char buffer[128];
        SPRINTF(buffer, "%s%s", directory, src);
        if(!file.open(buffer)){
            CHECK(false);
            return;
        }

        cppimg::OpenEXR::Information information;
        if(!cppimg::OpenEXR::read(information, CPPIMG_NULL, file)){
            CHECK(false);
            return;
        }

        cppimg::u8* image = new cppimg::u8[information.width_*information.height_*information.getBytesPerPixel()];
        if(cppimg::OpenEXR::read(information, image, file)){
            file.close();
            for(cppimg::s32 i=0; i<information.numChannels_; ++i){
              CHECK(information.types_[i] == static_cast<cppimg::s32>(cppimg::Type::HALF));
            }
            cppimg::OFStream ofile;
            SPRINTF(buffer, "%s%s", directory, dst);
            if(ofile.open(buffer)){
                bool result = cppimg::OpenEXR::write(ofile, information.width_, information.height_, information.colorType_, cppimg::Type::HALF, image);
                CHECK(result);
                ofile.close();
            }
            if(file.open(buffer)){
                cppimg::u8* image2 = new cppimg::u8[information.width_*information.height_*information.getBytesPerPixel()];
                if(cppimg::OpenEXR::read(information, image2, file)){
                    for(cppimg::s32 i=0; i<(information.width_*information.height_); ++i){
                        CHECK(image[i] == image2[i]);
                    }
                }
                delete[] image2;
                file.close();
            }
        }else{
            CHECK(false);
        }
        delete[] image;
    }
}

TEST_CASE("Read OpenEXR" "[EXR]")
{
    SECTION("nocompression"){
        load("OpenEXR/rgb_nocompression.exr", "rgb_nocompression.exr.bmp", "../data/");
        load("OpenEXR/rgba_nocompression.exr", "rgba_nocompression.exr.bmp", "../data/");
        load("OpenEXR/gray_nocompression.exr", "gray_nocompression.exr.bmp", "../data/");
    }

    SECTION("rle"){
        load("OpenEXR/rgb_rle.exr", "rgb_rle.exr.bmp", "../data/");
        load("OpenEXR/rgba_rle.exr", "rgba_rle.exr.bmp", "../data/");
        load("OpenEXR/gray_rle.exr", "gray_rle.exr.bmp", "../data/");
    }

    SECTION("zips"){
        load("OpenEXR/rgb_zips.exr", "rgb_zips.exr.bmp", "../data/");
        load("OpenEXR/rgba_zips.exr", "rgba_zips.exr.bmp", "../data/");
        load("OpenEXR/gray_zips.exr", "gray_zips.exr.bmp", "../data/");
    }

    SECTION("zip"){
        load("OpenEXR/rgb_zip.exr", "rgb_zip.exr.bmp", "../data/");
        load("OpenEXR/rgba_zip.exr", "rgba_zip.exr.bmp", "../data/");
        load("OpenEXR/gray_zip.exr", "gray_zip.exr.bmp", "../data/");
    }

    SECTION("zip"){
        save("OpenEXR/rgb_zip.exr", "rgb_zip.exr", "../data/");
        save("OpenEXR/rgba_zip.exr", "rgba_zip.exr", "../data/");
        save("OpenEXR/gray_zip.exr", "gray_zip.exr", "../data/");
    }
}
