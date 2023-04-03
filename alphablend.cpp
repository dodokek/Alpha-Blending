#include "include/alphablend.hpp"

// #define NO_AVX

void BlendMain()
{
    sf::Clock clock; // starts the clock

    AllFileInfo back_info = {};
    AllFileInfo front_info = {};
    ImgMainInfo result_img = {};

    ImgMainInfo background_img = HandleBmpFile (BackgroundImgPath, &back_info);
    ImgMainInfo foreground_img = HandleBmpFile (ForegroundImgPath, &front_info);

    while (true)
    {
        clock.restart();
        
        #ifdef NO_AVX
            result_img = BlendNoAvx (background_img, foreground_img);
        #else
            result_img = BlendAvx (background_img, foreground_img);
        #endif
        
        sf::Time elapsed_time = clock.getElapsedTime();
        printf ("kiloFrames per second: %.4f\n", 1/elapsed_time.asSeconds() / 100);
    }

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
            ARGB* backARGB =  (ARGB*) (back.pixel_array + cur_x + cur_y * back.width);
            ARGB* frontARGB = (ARGB*) (front.pixel_array + cur_x + cur_y * front.width);
            ARGB resultARGB = {};
            
            resultARGB.alpha = backARGB->alpha;
            resultARGB.red   = ( backARGB->red   * backARGB->alpha + frontARGB->red   * (255 - frontARGB->alpha) ) >> 8;
            resultARGB.green = ( backARGB->green * backARGB->alpha + frontARGB->green * (255 - frontARGB->alpha) ) >> 8;
            resultARGB.blue  = ( backARGB->blue  * backARGB->alpha + frontARGB->blue  * (255 - frontARGB->alpha) ) >> 8;

            result_img.pixel_array[cur_x + x_offset + (cur_y + y_offset) * result_img.width] = *((uint32_t*) &resultARGB);
           
            // printf ("Still alive x: %d, y: %d\n", cur_x, cur_y);
        }
    }

    return result_img; 
}


ImgMainInfo BlendAvx (ImgMainInfo back, ImgMainInfo front)
{
    __m128i _0 = _mm_set1_epi8 (0); 
    ImgMainInfo result_img = back;

    for (int cur_y = 0; cur_y < front.height; cur_y++)
    {
        for (int cur_x = 0; cur_x < front.width - 4; cur_x += 4)
        {

        // Step 1 - Loading the values in vectors
            __m128i FrontLow = _mm_loadu_si128 ((__m128i*)(front.pixel_array + cur_y * front.width + cur_x));
            __m128i BackLow  = _mm_loadu_si128 ((__m128i*)(back.pixel_array + cur_y * back.width + cur_x));
        

        // Step 2 - Separatiog bytes to gain more space for multiplication
            __m128i FrontHigh = (__m128i) _mm_movehl_ps ((__m128) _0, (__m128) FrontLow);
            __m128i BackHigh  = (__m128i) _mm_movehl_ps ((__m128) _0, (__m128) BackLow);
        
        // Step 3 - Preparing bytes for multiplication
            FrontLow  = _mm_cvtepi8_epi16 (FrontLow);
            FrontHigh = _mm_cvtepi8_epi16 (FrontLow);

            BackLow  = _mm_cvtepi8_epi16 (BackLow);
            BackHigh = _mm_cvtepi8_epi16 (BackHigh);
        
        // Step 4 - shuffling the alpha parametr
            __m128i shuffle_mask = _mm_set_epi8 (0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14,
                                                 0x80,  6, 0x80,  6, 0x80,  6, 0x80,  6);

            __m128i AlphaL  = _mm_shuffle_epi8 (FrontLow, shuffle_mask);
            __m128i AlphaH = _mm_shuffle_epi8  (FrontHigh, shuffle_mask);
        
        // Step 5 - Multiplying on alpha
            FrontLow  = _mm_mullo_epi16 (FrontLow, AlphaL);    
            FrontHigh = _mm_mullo_epi16 (FrontHigh, AlphaH);   

            BackLow  = _mm_mullo_epi16 (BackLow,  _mm_sub_epi16 (_mm_set1_epi16(255), AlphaL));    
            BackHigh = _mm_mullo_epi16 (BackHigh, _mm_sub_epi16 (_mm_set1_epi16(255), AlphaH));  

            __m128i SumLow  = _mm_add_epi16 (FrontLow, BackLow);     
            __m128i SumHigh = _mm_add_epi16 (FrontHigh, BackHigh);   
        
        // Step 6 - accumulation of results
            shuffle_mask = _mm_set_epi8 (0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
                                         15U,  13U,  11U,  9U,   7U,   5U,   3U,   1U);

            SumLow  = _mm_shuffle_epi8 (SumLow,  shuffle_mask);
            SumHigh = _mm_shuffle_epi8 (SumHigh, shuffle_mask);

            __m128i result = (__m128i) _mm_movelh_ps ((__m128) SumLow, (__m128) SumHigh);

            _mm_storeu_si128 ((__m128i*) (result_img.pixel_array + cur_x + cur_y * result_img.width), result);

            // printf ("Didn't die yet x:%d y:%d\n", cur_x, cur_y);
        }
    }

    return result_img;

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





