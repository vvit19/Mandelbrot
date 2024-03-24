#include "mandelbrot.hpp"

static void DrawMandelbrotAvx (sf::Image &image, float offset_x, float offset_y, float scale);
static void DrawMandelbrot    (sf::Image &image, float offset_x, float offset_y, float scale);
static int  CmpVector         (__m256 vector_1, __m256 vector_2, __m256i* iterations_vector, uint32_t op_mask);

void StartDrawing ()
{
    float offset_x = 0;
    float offset_y = 0;
    float scale    = 0.005f;

    sf::RenderWindow window (sf::VideoMode (WIDTH, HEIGHT), "Mandelbrot");

    sf::Image image;
    image.create (WIDTH, HEIGHT, sf::Color::Black);

    sf::Texture texture = {};
    texture.loadFromImage (image);

    sf::Sprite sprite = {};
    sprite.setTexture (texture);

    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            offset_x -= 20.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            offset_x += 20.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            offset_y -= 20.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            offset_y += 20.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen))
            scale *= 1.25f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
            scale /= 1.25f;

        sf::Event event = {};
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

static void DrawMandelbrotAvx (sf::Image &image, float offset_x, float offset_y, float scale)
{
    float center_x = WIDTH  / 2 + offset_x;
    float center_y = HEIGHT / 2 + offset_y;

    __m256 center_x_vector   = _mm256_set1_ps (center_x);
    __m256 scale_vector      = _mm256_set1_ps (scale);
    __m256 max_radius_vector = _mm256_set1_ps (MAX_RADIUS);
    __m256 offset_vector     = _mm256_set_ps  (0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);

    for (int cur_y = 0; cur_y < HEIGHT; ++cur_y)
    {
        __m256 c_im_vector = _mm256_set1_ps (((float) cur_y - center_y) * scale);

        for (int cur_x = 0; cur_x < WIDTH; cur_x += VECTOR_SIZE)
        {
            __m256 c_real_vector = _mm256_add_ps  (offset_vector, _mm256_set1_ps ((float) cur_x));
                   c_real_vector = _mm256_sub_ps  (c_real_vector, center_x_vector);
                   c_real_vector = _mm256_mul_ps  (c_real_vector, scale_vector);

            int i = 0;
            __m256i iterations_vector = _mm256_set1_epi32 (0);
            for (__m256 vector_x = c_real_vector, vector_y = c_im_vector ; i < MAX_ITERATIONS; ++i)
            {
                __m256 x_pow_vector = _mm256_mul_ps (vector_x, vector_x);
                __m256 y_pow_vector = _mm256_mul_ps (vector_y, vector_y);
                __m256 xy_vector    = _mm256_mul_ps (vector_x, vector_y);

                __m256 vector_cur_radius = _mm256_add_ps (x_pow_vector, y_pow_vector);
                if (!CmpVector (vector_cur_radius, max_radius_vector, &iterations_vector, _CMP_GE_OQ)) break;

                vector_x = _mm256_add_ps (x_pow_vector, _mm256_sub_ps (c_real_vector, y_pow_vector));
                vector_y = _mm256_add_ps (c_im_vector,  _mm256_add_ps (xy_vector, xy_vector));  // Z_n = (Z_{n-1})^2 + C_0
            }

            uint32_t* iterations_array = (uint32_t*) (&iterations_vector);
            for (int offset = 0; offset < VECTOR_SIZE; ++offset)
            {
                if (iterations_array[offset] % 2 == 1) image.setPixel (cur_x + offset, cur_y, sf::Color::White);
                else image.setPixel (cur_x, cur_y, sf::Color::Black);
            }
        }
    }
}

static void DrawMandelbrot (sf::Image &image, float offset_x, float offset_y, float scale)
{
    float center_x = WIDTH  / 2 + offset_x;
    float center_y = HEIGHT / 2 + offset_y;

    for (int cur_y = 0; cur_y < HEIGHT; ++cur_y)
    {
        float c_im = ((float) cur_y - center_y) * scale;

        for (int cur_x = 0; cur_x < WIDTH; ++cur_x)
        {
            float c_real = ((float) cur_x - center_x) * scale;

            int i = 0;
            for (float x = c_real, y = c_im; i < MAX_ITERATIONS; ++i)
            {
                float x_pow = x * x;
                float y_pow = y * y;
                float xy    = x * y;

                float cur_radius = x_pow + y_pow;
                if (cur_radius >= MAX_RADIUS) break;

                x = x_pow - y_pow + c_real;
                y = xy + xy + c_im;             // Z_n = (Z_{n-1})^2 + C_0
            }

            if (i % 2 == 1) image.setPixel (cur_x, cur_y, sf::Color::White);
            else image.setPixel (cur_x, cur_y, sf::Color::Black);
        }
    }
}

static int CmpVector (__m256 vector_1, __m256 vector_2, __m256i* iterations_vector, uint32_t op_mask)
{
    __m256 res = _mm256_cmp_ps (vector_1, vector_2, _CMP_LT_OQ);
    *iterations_vector = _mm256_sub_epi32 (*iterations_vector, _mm256_castps_si256 (res));

   return _mm256_movemask_ps (res);
}
