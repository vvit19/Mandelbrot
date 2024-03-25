#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#define AVX
// #define CHECK_PERFORMANCE

#include <SFML/Graphics.hpp>
#include <cassert>
#include <immintrin.h>

const int   MAX_ITERATIONS = 256;
const float MAX_RADIUS     = 100.f;
const int   WIDTH          = 800;
const int   HEIGHT         = 600;
const int   VECTOR_SIZE    = 8;
const int   PERFORMANCE_ITERATIONS = 300;

void StartDrawing ();

#endif
