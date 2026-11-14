#include "grid.hpp"
#include <cstdio>
#include <stdexcept>

Grid::Grid(int width, int height) {
    width_ = width;
    height_ = height;

    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("width and height must be greater than 0");
    }
}

