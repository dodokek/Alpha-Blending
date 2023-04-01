#ifndef ALPHA_H
#define ALPHA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(push, 1)

struct Header 
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


void BlendMain();

void dispBmpInfo(char *filename);

#pragma pack(pop)

#endif