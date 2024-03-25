# Mandelbrot set drawing
## Описание проекта
В данном проекте реализовано построение графика для множества Мандельброта наиболее эффективным способом. Для построения использовалась графическая библиотека SFML,
для увеличения производительности использовались AVX инструкции.

<img src= "https://github.com/vvit19/Mandelbrot/blob/master/img/image.png" width="500px"/>

## Что такое множество Мандельброта?
Точки множества Мандельброта удовлетворяют рекуррентному соотношению:
$Z_{n + 1} = Z_n ^ 2 + C_0$, где $Z_0 = 0$, $Z_{i} = X_{i} + i Y_{i}$ ($Z$ - точка на комплексной плоскости), $C_{0}$ - начальная точка (с координатами $x_0$, $y_0$).

Цвет, в который закрашивалась точка, зависел от i - номера итерации, на котором завершился цикл "подсчета точки" (код снизу).

~~~C++
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

if (i % 2 == 1) pixels_array[pixels_cnt++] = sf::Color::White;
else pixels_array[pixels_cnt++] = sf::Color::Black;
~~~

## Использование AVX инструкций
AVX инструкции позволяют вместо последовательного подсчета каждой точки, обрабатывать целый вектор (8 точек) одновременно, таким образом максимальное число итераций цикла может уменьшиться в 8 раз! Посмотрим на цикл подсчета точки:
~~~ C++
for (__m256 vector_x = c_real_vector, vector_y = c_im_vector; i < MAX_ITERATIONS; ++i)
{
    __m256 x_pow_vector = _mm256_mul_ps (vector_x, vector_x);
    __m256 y_pow_vector = _mm256_mul_ps (vector_y, vector_y);
    __m256 xy_vector    = _mm256_mul_ps (vector_x, vector_y);

    __m256 vector_cur_radius = _mm256_add_ps (x_pow_vector, y_pow_vector);
    if (!CmpVector (vector_cur_radius, max_radius_vector, &iterations_vector)) break;

    vector_x = _mm256_add_ps (x_pow_vector, _mm256_sub_ps (c_real_vector, y_pow_vector));
    vector_y = _mm256_add_ps (c_im_vector,  _mm256_add_ps (xy_vector, xy_vector));
}
~~~

## Анализ производительности
Измерим время работы функций (в миллисекундах) подсчета точек (с AVX/без AVX) и проанализируем полученные результаты:

|             | No flags          | -O3            |
| ------      | :---------------: | :------------: |
| No AVX      | 106,188           | 52,132         |
| AVX         | 32,665            | 8.032          |

Видим, что даже без флагов оптимизации, использование функции с AVX инструкциями быстрее, чем использование обычной функции с -О3. А использование AVX функции с -О3 так вообще обгоняет обычную функцию с -О3 в 6,5 раз!

Вывод: используйте AVX)
