#include "include/alphablend.hpp"

void BlendMain()
{
    sf::Clock clock; // starts the clock

    AllFileInfo back_info = {};
    AllFileInfo front_info = {};
    ImgMainInfo result_img = {};

    ImgMainInfo background_img = HandleBmpFile (BackgroundImgPath, &back_info);
    ImgMainInfo foreground_img = HandleBmpFile (ForegroundImgPath, &front_info);

    // for (int i = 0; i < 10000000; i++)
    // {
        clock.restart();
        
        result_img = BlendNoAvx (background_img, foreground_img);
        
        sf::Time elapsed_time = clock.getElapsedTime();
        printf ("kiloFrames per second: %.2f\n", 1/elapsed_time.asSeconds() / 1000);
    // }


    LoadResultImg (result_img, back_info);


    free (background_img.pixel_array);
    free (foreground_img.pixel_array);
}


ImgMainInfo BlendNoAvx (ImgMainInfo back, ImgMainInfo front)
{
    ImgMainInfo result_img = back;

    ARGB frontARGB = {};
    ARGB backARGB = {};

    for (uint32_t cur_x = 0; cur_x < front.width; cur_x++)
    {
        for (uint32_t cur_y = 0; cur_y < front.height; cur_y++)
        {
            uint32_t back_pixel = back.pixel_array[cur_x + cur_y * back.width];
            uint32_t front_pixel = front.pixel_array[cur_x + cur_y * front.width];

            backARGB.alpha = (unsigned char)   (back_pixel >> (8 * 3));
            backARGB.red =   (unsigned char)   (back_pixel >> (8 * 2));
            backARGB.green = (unsigned char)   (back_pixel >> (8 * 1));
            backARGB.blue =  (unsigned char)   (back_pixel >> (8 * 0));

            frontARGB.alpha = (unsigned char)   (front_pixel >> (8 * 3));
            frontARGB.red =   (unsigned char)   (front_pixel >> (8 * 2));
            frontARGB.green = (unsigned char)   (front_pixel >> (8 * 1));
            frontARGB.blue =  (unsigned char)   (front_pixel >> (8 * 0));

            ARGB resultARGB = {};
            
            resultARGB.alpha = backARGB.alpha;
            resultARGB.red   =   ( backARGB.red   * backARGB.alpha + frontARGB.red   * (255 - backARGB.alpha) ) >> 8;
            resultARGB.green =   ( backARGB.green * backARGB.alpha + frontARGB.green * (255 - backARGB.alpha) ) >> 8;
            resultARGB.blue  =   ( backARGB.blue  * backARGB.alpha + frontARGB.blue  * (255 - backARGB.alpha) ) >> 8;


            result_img.pixel_array[cur_x + cur_y * result_img.width * 3]  = ( resultARGB.alpha  << 3 * 8 ) +
                                                                            ( resultARGB.red    << 2 * 8  ) +
                                                                            ( resultARGB.green  << 1 * 8  ) + 
                                                                            ( resultARGB.blue   << 0 * 8  );

        }
    }

    return result_img; 
}


ImgMainInfo BlendAvx (ImgMainInfo back, ImgMainInfo front)
{

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





