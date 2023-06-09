#include "include/alphablend.hpp"

#define NO_AVX

void DrawMain ()
{
    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "Mandelbebra");
    window.setFramerateLimit(30);

    sf::Image canvas;
    canvas.loadFromFile (BackgroundImgPath);

    sf::Image foregorund;
    foregorund.loadFromFile (ForegroundImgPath);

    sf::Texture texture;
    texture.loadFromImage( canvas );

    sf::Sprite sprite;
    sprite.setTexture( texture );
    BlendMain (foregorund, canvas);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {        
                window.close();
            }
        }

        
        texture.update( canvas );
        window.clear();
        window.draw ( sprite );
        window.display();
    }

}



void BlendMain(sf::Image& foreground_img, sf::Image& background_img)
{
    sf::Clock clock; // starts the clock

    // while (true)
    {
        clock.restart();
        
        #ifdef NO_AVX
            BlendNoAvx (foreground_img, background_img);
        #else
            BlendAvx (foreground_img, background_img);
        #endif
        
        sf::Time elapsed_time = clock.getElapsedTime();
        printf ("Frames per second: %.4f\n", 1/elapsed_time.asSeconds());
    }

}


void BlendNoAvx (sf::Image& front, sf::Image& back)
{
    // printf ("Front: %d, %d\n", front.getSize().x, front.getSize().y);
    // printf ("Back: %d, %d\n",back.getSize().x, back.getSize().y);

    uint32_t* background_pixels =  (uint32_t*) back.getPixelsPtr(); 
    uint32_t* foreground_pixels =  (uint32_t*) front.getPixelsPtr(); 

    uint32_t front_w = front.getSize().x;
    uint32_t front_h = front.getSize().y;

    uint32_t back_w = back.getSize().x;
    uint32_t back_h = back.getSize().y;

    // for (int i = 0; i < 1000; i++)
    {

    for (uint32_t cur_x = 0; cur_x < front_w; cur_x++)
    {
        for (uint32_t cur_y = 0; cur_y < front_h; cur_y++)
        {
                ARGB* front_color = (ARGB*) (foreground_pixels + cur_x + cur_y * front_w);
                ARGB* back_color =  (ARGB*) (background_pixels + cur_x + x_offset + (cur_y + y_offset) * back_w);
                
                // back_color->alpha = (front_color->alpha + back_color->alpha)/2;
                back_color->red   = (front_color->red    * front_color->alpha + back_color->red   * (255 - front_color->alpha)) >> 8;
                back_color->green = (front_color->green  * front_color->alpha + back_color->green * (255 - front_color->alpha)) >> 8;
                back_color->blue  = (front_color->blue   * front_color->alpha + back_color->blue  * (255 - front_color->alpha)) >> 8;
        }
    }

    }

}


void BlendAvx (sf::Image& front, sf::Image& back)
{
    // printf ("Front: %d, %d\n", front.getSize().x, front.getSize().y);
    // printf ("Back: %d, %d\n",back.getSize().x, back.getSize().y);

    __m128i _m_zero = _mm_set1_epi8 (0); 

    uint32_t* background_pixels =  (uint32_t*) back.getPixelsPtr(); 
    uint32_t* foreground_pixels =  (uint32_t*) front.getPixelsPtr(); 

    uint32_t front_w = front.getSize().x;
    uint32_t front_h = front.getSize().y;

    uint32_t back_w = back.getSize().x;

    // for (int i = 0; i < 1000; i++)
    {

    for (int cur_x = 0; cur_x < front_w; cur_x += 4)
    {
        for (int cur_y = 0; cur_y < front_h; cur_y++)
        {

        // Step 1 - Loading the values in vectors
            __m128i ForegrL = _mm_loadu_si128 ((__m128i*)(foreground_pixels + cur_y * front_w + cur_x));
            __m128i BackgrL = _mm_loadu_si128 ((__m128i*)(background_pixels + cur_x + x_offset + (cur_y + y_offset) * back_w));
        
        // Step 2 - Separating bytes on halfs to gain more space for multiplication
            __m128i ForegrH = (__m128i) _mm_movehl_ps ((__m128) _m_zero, (__m128) ForegrL);
            __m128i BackgrH = (__m128i) _mm_movehl_ps ((__m128) _m_zero, (__m128) BackgrL);
        
        // Step 3 - Preparing bytes for multiplication by setting 1 byte space between 'em
            ForegrL  = _mm_cvtepi8_epi16 (ForegrL);
            ForegrH  = _mm_cvtepi8_epi16 (ForegrH);

            BackgrL  = _mm_cvtepi8_epi16 (BackgrL);
            BackgrH  = _mm_cvtepi8_epi16 (BackgrH);
        
        // Step 4 - shuffling the alpha parametr
            __m128i shuffle_mask = _mm_set_epi8 (0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14,
                                                 0x80,  6, 0x80,  6, 0x80,  6, 0x80,  6);

            __m128i AlphaL  = _mm_shuffle_epi8 (ForegrL, shuffle_mask);
            __m128i AlphaH  = _mm_shuffle_epi8 (ForegrH, shuffle_mask);
        
        // Step 5 - Multiplying on alpha
            ForegrL  = _mm_mullo_epi16 (ForegrL, AlphaL);    
            ForegrH  = _mm_mullo_epi16 (ForegrH, AlphaH);   

            BackgrL  = _mm_mullo_epi16 (BackgrL,  _mm_sub_epi16 (_mm_set1_epi16(255), AlphaL));    
            BackgrH  = _mm_mullo_epi16 (BackgrH, _mm_sub_epi16 (_mm_set1_epi16(255), AlphaH));  

            __m128i SumLow  = _mm_add_epi16 (ForegrL, BackgrL);     
            __m128i SumHigh = _mm_add_epi16 (ForegrH, BackgrH);   
        
        // Step 6 - accumulation of results
            shuffle_mask = _mm_set_epi8 (0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
                                         15U,  13U,  11U,  9U,   7U,   5U,   3U,   1U);

            SumLow  = _mm_shuffle_epi8 (SumLow,  shuffle_mask);
            SumHigh = _mm_shuffle_epi8 (SumHigh, shuffle_mask);

        // Step 7 - Storing the result in SFML pixel array to draw it on the screen
            __m128i result = (__m128i) _mm_movelh_ps ((__m128) SumLow, (__m128) SumHigh);


            _mm_storeu_si128 ((__m128i*) (background_pixels + cur_x + x_offset + (cur_y + y_offset) * back_w), result);

            // printf ("Hasn't died yet x:%d y:%d\n", cur_x, cur_y);
        }
      
    }

    }


}







