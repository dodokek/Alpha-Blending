#include "include/alphablend.hpp"

void BlendMain()
{
    FILE* img_file = fopen ("img/Table.bmp", "rb");

    Header     img_header = {};
    BitmapInfo img_bitmap = {};
    ARGB_info  img_argb   = {};

    fseek (img_file, 0, SEEK_SET);
    
    fread (&img_header, 1, sizeof (img_header), img_file);
    
    fread (&img_bitmap, 1, sizeof (img_bitmap), img_file);

    fread (&img_argb, 1, sizeof (img_argb), img_file);

    printf ("Width: %d, Height: %d\n", img_bitmap.bi_width, img_bitmap.bi_height);

    fclose (img_file);
}



