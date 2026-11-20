#include "grid.hpp"

#include <algorithm>
#include <cstdio>
#include <map>
#include <stdexcept>
#include <vector>
#include <cmath>

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

  for (int y = height_-1; y >= 0; y--) {
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

void Grid::drawLine(Point start, Point end) {
  std::vector<Point> linePixels;
  linePixels = rasterizeLine(start, end);

  for (int i = 0; i < linePixels.size(); i++) {
    setPixel(linePixels[i], true);
  }
}

void Grid::drawPolygon(Polygon polygon) {
  int pointCount = polygon.points.size();

  if (pointCount < 3) {
    throw std::invalid_argument("Polygon requires at least 3 vertices");
  }

  for (int i = 0; i < pointCount; ++i) {
    Point& start = polygon.points[i];
    Point& end = polygon.points[(i + 1) % pointCount];
    drawLine(start, end);
  }
}

void Grid::lineFillPolygon(Polygon polygon) {
  int pointCount = polygon.points.size();

  if (pointCount < 3) {
    throw std::invalid_argument("Polygon requires at least 3 vertices");
  }

  std::map<int, std::vector<double>> intersections;
  for (int i = 0; i < pointCount; ++i) {
    Point& start = polygon.points[i];
    Point& end = polygon.points[(i + 1) % pointCount];

    int startY = std::min(start.y, end.y);
    int endY = std::max(end.y, start.y);
    for (int y = startY; y < endY; y++) {
      if (start.y == end.y) {
        continue;
      }
      double x = start.x + (y - start.y) * (end.x - start.x) / (end.y - start.y);
      intersections[y].push_back(x);
    }
  }

  for (auto inter = intersections.begin(); inter != intersections.end(); ++inter) {
    int y = inter->first;
    std::vector<double> value = inter->second;
    std::sort(value.begin(), value.end());

    for (int i = 0; i < value.size()-1; i += 2) {
      int startX = std::ceil(value[i]);
      int endX = std::floor(value[i+1]);

      for (int x = startX; x <= endX; x++) {
        setPixel(Point(x, y), true);
      }
    }
  }
}