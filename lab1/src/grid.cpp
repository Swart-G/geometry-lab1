#include "grid.hpp"
#include <cstdio>
#include <stdexcept>
#include <vector>

Grid::Grid(int width, int height) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument("Width and height must be greater than 0");
  }

  width_ = width;
  height_ = height;

  grid_.assign(width_, std::vector<char>(height_, emptyPixel));
}

Grid::~Grid() = default;

int Grid::getHeight() const { return height_; }

int Grid::getWidth() const { return width_; }

void Grid::render() const {
  for (int x = 0; x < width_+2; x++) {
    printf("%c", framePixel);
  }
  printf("\n");

  for (int y = 0; y < height_; y++) {
    printf("%c", framePixel);

    for (int x = 0; x < width_; x++) {
      printf("%c", grid_[x][y]);
    }

    printf("%c\n", framePixel);
  }

  for (int x = 0; x < width_+2; x++) {
    printf("%c", framePixel);
  }
  printf("\n");
}

void Grid::clear() {
  grid_.assign(width_, std::vector<char>(height_, emptyPixel));
}

void Grid::setPixel(Point point, bool value) {
  if (point.x >= width_ || point.x < 0) {
    throw std::invalid_argument("Coordinate x is out of bounds");
  }
  if (point.y >= height_ || point.y < 0 ) {
    throw std::invalid_argument("Coordinate y is out of bounds");
  }

  if (value) {
    grid_[point.x][point.y] = filledPixel;
  } else {
    grid_[point.x][point.y] = emptyPixel;
  }
}

std::vector<Point> Grid::rasterizeLine(Point start, Point end) const {
  if (start.x >= width_ || start.x < 0) {
    throw std::invalid_argument("Start x coordinate is out of bounds");
  }
  if (start.y >= height_ || start.y < 0 ) {
    throw std::invalid_argument("Start y coordinate is out of bounds");
  }

  if (end.x >= width_ || end.x < 0) {
    throw std::invalid_argument("End x coordinate is out of bounds");
  }
  if (end.y >= height_ || end.y < 0 ) {
    throw std::invalid_argument("End y coordinate is out of bounds");
  }

  int stepX = end.x >= start.x ? 1 : -1;
  int stepY = end.y >= start.y ? 1 : -1;

  std::vector<Point> linePixels;
  Point current = start;

  int diffFirstAxis = std::max(abs(start.x - end.x), abs(start.y - end.y));
  int diffSecondAxis = std::min(abs(start.x - end.x), abs(start.y - end.y));
  int error = 2 * diffSecondAxis - diffFirstAxis;
  bool xIsMain = abs(start.x - end.x) >= abs(start.y - end.y);

  linePixels.push_back(start);
  while (current.x != end.x || current.y != end.y) {
    if (error < 0) {
      if (xIsMain) {
        current.x += stepX;
      } else {
        current.y += stepY;
      }

      error += 2 * diffSecondAxis;
    } else if (error >= 0) {
      current.x += stepX;
      current.y += stepY;

      error += 2 * diffSecondAxis - 2 * diffFirstAxis;
    }

    linePixels.push_back(current);
  }

  return linePixels;
}

