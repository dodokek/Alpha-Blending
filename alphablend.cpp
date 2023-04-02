#include "include/alphablend.hpp"

void BlendMain()
{
    AllFileInfo back_info = {};
    AllFileInfo front_info = {};

    ImgMainInfo background_img = HandleBmpFile (BackgroundImgPath, &back_info);
    ImgMainInfo foreground_img = HandleBmpFile (ForegroundImgPath, &front_info);

    // DisplayImage (background_img);


    LoadResultImg (background_img, back_info);


    free (background_img.pixel_array);
    free (foreground_img.pixel_array);
}


ImgMainInfo HandleBmpFile (const char* name, AllFileInfo* info_to_save)
{
    FILE* img_file = fopen (name, "rb");

    BitmapHeader     header    = {};
    BitmapInfo       bitmap    = {};
    ARGB_info        argb_info = {};

    fseek (img_file, 0, SEEK_SET);


    // Reading all header information such as width height and 30 unneeded params
    fread (&header, 1, sizeof (header), img_file);
    fread (&bitmap, 1, sizeof (bitmap), img_file);
    fread (&argb_info, 1, sizeof (argb_info), img_file);

    printf ("File type: %x\n", header.file_type);

    // Moving all important info to one struct
    ImgMainInfo cur_img = {};

    cur_img.bi_size = bitmap.bi_size;
    cur_img.size    = header.file_size;
    cur_img.offset  = header.file_size;
    cur_img.height  = bitmap.bi_height;
    cur_img.width   = bitmap.bi_width;

    cur_img.pixel_array = (uint32_t*) calloc (cur_img.height * cur_img.width,
                                              sizeof (uint32_t));

    // Reading pixels array
    int img_size = fread(cur_img.pixel_array, (size_t) (cur_img.width * cur_img.height), sizeof(uint32_t), img_file);

    printf ("Successfully read BMP file \"%s\" "
            "Width: %d, Height: %d\n", 
            name, bitmap.bi_width, bitmap.bi_height);

    fclose (img_file);

    // Storing gmp params

    info_to_save->bitmap_header = header;
    info_to_save->bitmap_info = bitmap;
    info_to_save->argb_info = argb_info;

    return cur_img;
}


void LoadResultImg (ImgMainInfo& image, AllFileInfo general_header)
{
    FILE *bmp_file = fopen(ResultImgPath, "wb");
   
    fwrite (&general_header, 1, sizeof (general_header), bmp_file);

    fwrite (image.pixel_array, (size_t) (image.width * image.height), 
                                sizeof (uint32_t), bmp_file);

    fclose(bmp_file);

}


void DisplayImage (ImgMainInfo image)
{

    for (uint32_t cur_x = 0; cur_x < image.width; cur_x++)
    {
        for (uint32_t cur_y = 0; cur_y < image.height; cur_y++)
        {
            uint32_t cur_pixel = image.pixel_array[cur_x + ( image.height - cur_y - 1 ) * image.width];

            image.pixel_array[cur_x + cur_y * image.width] += 1;

        }
    }
    
}


