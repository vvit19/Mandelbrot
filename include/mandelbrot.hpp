#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

// #define AVX

#include <SFML/Graphics.hpp>
#include <cassert>

const int   MAX_ITERATIONS = 256;
const float MAX_RADIUS     = 100.f;
const int   WIDTH          = 800;
const int   HEIGHT         = 600;

void StartDrawing ();
void DrawMandelbrotAvx (sf::Image &image, float offset_x, float offset_y, float scale);
void DrawMandelbrot (sf::Image &image, float offset_x, float offset_y, float scale);

#endif
