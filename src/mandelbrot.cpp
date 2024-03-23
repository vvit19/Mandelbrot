#include "mandelbrot.hpp"

void StartDrawing ()
{
    float offset_x = 0;
    float offset_y = 0;
    float scale    = 0.005f;

    sf::RenderWindow window (sf::VideoMode (WIDTH, HEIGHT), "Mandelbrot");

    sf::Image image;
    image.create (WIDTH, HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.loadFromImage (image);

    sf::Sprite sprite;
    sprite.setTexture (texture);

    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            offset_x -= 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            offset_x += 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            offset_y -= 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            offset_y += 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
            scale *= 1.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen))
            scale /= 1.1f;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        #ifdef AVX
            DrawMandelbrotAvx (image, offset_x, offset_y, scale);
        #else
            DrawMandelbrot (image, offset_x, offset_y, scale);
        #endif

        texture.update (image);
        window.clear ();
        window.draw (sprite);
        window.display ();
    }
}

void DrawMandelbrotAvx (sf::Image &image, float offset_x, float offset_y, float scale)
{

}

void DrawMandelbrot (sf::Image &image, float offset_x, float offset_y, float scale)
{
    float center_x = WIDTH  / 2 + offset_x;
    float center_y = HEIGHT / 2 + offset_y;
    float c_im = 0.f, c_real = 0.f, x_pow = 0.f, y_pow = 0.f, xy = 0, cur_radius = 0;

    for (int cur_y = 0; cur_y < HEIGHT; ++cur_y)
    {
        c_im = ((float) cur_y - center_y) * scale;

        for (int cur_x = 0; cur_x < WIDTH; ++cur_x)
        {
            c_real = ((float) cur_x - center_x) * scale;

            int i = 0;
            for (float x = c_real, y = c_im; i < MAX_ITERATIONS; ++i)
            {
                x_pow = x * x;
                y_pow = y * y;
                xy    = x * y;

                cur_radius = x_pow + y_pow;
                if (cur_radius >= MAX_RADIUS) break;

                x = x_pow - y_pow + c_real;
                y = xy + xy + c_im;             // Z_n = Z_{n-1} + C_0
            }

            if (i % 2 == 1) image.setPixel (cur_x, cur_y, sf::Color::White);
            else image.setPixel (cur_x, cur_y, sf::Color::Black);
        }
    }
}
