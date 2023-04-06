#ifndef ALPHA_H
#define ALPHA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#pragma pack(push, 1)

struct BitmapHeader 
{
    uint16_t file_type;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

struct BitmapInfo 
{
    uint32_t bi_size;
    uint32_t bi_width;
    uint32_t bi_height;
    uint16_t bi_planes;
    uint16_t bi_bitcount;
    uint32_t bi_compression;
    uint32_t bi_size_image;
    int32_t  bi_xpxl_per_meter;
    int32_t  bi_ypxl_per_meter;
    uint32_t bi_clr_used;
    uint32_t bi_clr_important;
};


struct  ARGB_info 
{
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t alpha_mask;
    uint32_t window_color_space;
    uint32_t reserved[16];
};

#pragma pack(pop)

struct ImgMainInfo
{
    uint32_t bi_size;
    uint32_t size;
    uint32_t offset;

    uint32_t width;
    uint32_t height;

    uint32_t* pixel_array;
};

struct AllFileInfo
{
    BitmapHeader bitmap_header;
    BitmapInfo   bitmap_info;
    ARGB_info    argb_info;
};


struct ARGB
{
    unsigned char alpha;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

//--------------------------------------------------

const char BackgroundImgPath[] = "img/Table.bmp";
const char ForegroundImgPath[] = "img/Cat.bmp";
const char ResultImgPath[]     = "img/result.bmp";

const int x_offset = 250;
const int y_offset = 200;


const int W_WIDTH = 800;
const int W_HEIGHT = 600;


enum RGB_INDEX
{
    RED = 0, 
    GREEN,
    BLUE,
    ALPHA
};

//--------------------------------------------------


ImgMainInfo HandleBmpFile (const char* name, AllFileInfo* info_to_save);

void BlendMain(sf::Image& foreground_img, sf::Image& background_img);

void BlendNoAvx (sf::Image& front, sf::Image& back);

void BlendAvx (sf::Image& front, sf::Image& back);

void LoadResultImg (ImgMainInfo& image, AllFileInfo general_header);


void DrawMain ();

#endif