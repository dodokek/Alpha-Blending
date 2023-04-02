#include "include/alphablend.hpp"

void BlendMain()
{
    ImgMainInfo background_img = HandleBmpFile (BackgroundImgPath);
    ImgMainInfo foreground_img = HandleBmpFile (ForegroundImgPath);

    sf::RenderWindow window (sf::VideoMode(1500, 800), "Alpha Bebring");
    window.setFramerateLimit (30);

    sf::Image canvas;
    canvas.create (background_img.width, background_img.height, sf::Color::White );

    sf::Texture texture;
    texture.loadFromImage (canvas);
    texture.setRepeated (false);

    sf::Sprite sprite;
    sprite.setTexture (texture);

        DisplayImage (canvas, background_img);
        // canvas.saveToFile ("img/result.bmp");
    // while (window.isOpen())
    // {
    //     sf::Event event;
    //     while (window.pollEvent(event))
    //     {
    //         if (event.type == sf::Event::Closed)
    //         {        
    //             window.close();
    //         }
    //     }

    //     // DisplayImage (canvas, foreground_img);


    //     texture.update( canvas );
    //     window.clear();
    //     window.draw ( sprite );

    //     window.display();
    // }

    free (background_img.pixel_array);
    free (foreground_img.pixel_array);

}

ImgMainInfo HandleBmpFile (const char* name)
{
    FILE* img_file = fopen (name, "rb");

    BitmapHeader     header = {};
    BitmapInfo bitmap = {};
    ARGB_info  argb_info   = {};

    fseek (img_file, 0, SEEK_SET);

    // Reading all header information such as width height and 30 unneeded params
    fread (&header, 1, sizeof (header), img_file);
    fread (&bitmap, 1, sizeof (bitmap), img_file);
    fread (&argb_info, 1, sizeof (argb_info), img_file);

    // Moving all important info to one struct
    ImgMainInfo cur_img = {};

    cur_img.height = bitmap.bi_height;
    cur_img.width = bitmap.bi_width;

    cur_img.pixel_array = (uint32_t*) calloc (cur_img.height * cur_img.width,
                                              sizeof (uint32_t));

    // Reading pixels array
    int img_size = fread(cur_img.pixel_array, (size_t) (cur_img.width * cur_img.height), sizeof(uint32_t), img_file);

    printf ("Successfully read BMP file \"%s\" "
            "Width: %d, Height: %d\n", 
            name, bitmap.bi_width, bitmap.bi_height);

    fclose (img_file);

    return cur_img;
}


void DisplayImage (sf::Image& canvas, ImgMainInfo image)
{

    for (uint32_t cur_x = 0; cur_x < image.width; cur_x+=4)
    {
        for (uint32_t cur_y = 0; cur_y < image.height; cur_y+=4)
        {
            uint32_t cur_pixel = image.pixel_array[cur_x + ( image.height - cur_y - 1 ) * image.width];

            unsigned char alpha = (unsigned char)   (cur_pixel >> (8 * 3));
            unsigned char red =   (unsigned char)   (cur_pixel >> (8 * 2));
            unsigned char green = (unsigned char)   (cur_pixel >> (8 * 1));
            unsigned char blue =  (unsigned char)   (cur_pixel >> (8 * 0));

            // printf ("Red: %d, Green: %d, Blue: %d, Alpha: %d\n", red, green, blue, alpha);

            sf::Color color = sf::Color{red, green, blue, alpha};
            canvas.setPixel (cur_x, cur_y, color);
        }
    }
    
}


