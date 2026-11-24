#include "grid.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

Grid::Grid(int width, int height) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument("Width and height must be greater than 0");
  }

  constexpr int MAX_WIDTH = 40;
  constexpr int MAX_HEIGHT = 20;
  if (width > MAX_WIDTH || height > MAX_HEIGHT) {
    throw std::invalid_argument("Grid size must not exceed 40x20");
  }

  width_ = width;
  height_ = height;

  grid_.assign(width_, std::vector<char>(height_, emptyPixel));
}

Grid::~Grid() = default;

int Grid::getHeight() const { return height_; }

int Grid::getWidth() const { return width_; }

int Grid::getSteps() const { return lastStats_.steps; }

int Grid::getFilledPixels() const { return lastStats_.filledPixels; }

int Grid::getLineLength() const { return lastStats_.lineLength; }

void Grid::render() const {
  for (int x = 0; x < width_ + 2; x++) {
    printf("%c", framePixel);
  }
  printf("\n");

  for (int y = height_ - 1; y >= 0; y--) {
    printf("%c", framePixel);

    for (int x = 0; x < width_; x++) {
      printf("%c", grid_[x][y]);
    }

    printf("%c\n", framePixel);
  }

  for (int x = 0; x < width_ + 2; x++) {
    printf("%c", framePixel);
  }
  printf("\n");
}

void Grid::clear() {
  lastStats_ = {}; // stats reset

  grid_.assign(width_, std::vector<char>(height_, emptyPixel));
}

void Grid::setPixel(Point point, bool value) {
  if (point.x >= width_ || point.x < 0) {
    throw std::invalid_argument("Coordinate x is out of bounds");
  }
  if (point.y >= height_ || point.y < 0) {
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
  if (start.y >= height_ || start.y < 0) {
    throw std::invalid_argument("Start y coordinate is out of bounds");
  }

  if (end.x >= width_ || end.x < 0) {
    throw std::invalid_argument("End x coordinate is out of bounds");
  }
  if (end.y >= height_ || end.y < 0) {
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
  lastStats_ = {}; // stats reset
  std::vector<Point> linePixels;
  linePixels = rasterizeLine(start, end);

  int filledPixels = 0; // stats

  for (int i = 0; i < linePixels.size(); i++) {
    if (grid_[linePixels[i].x][linePixels[i].y] == emptyPixel) {
      filledPixels++;
    } // stats

    setPixel(linePixels[i], true);
  }

  // stats
  lastStats_.lineLength = linePixels.size();
  lastStats_.steps = linePixels.size() - 1;
  lastStats_.filledPixels = filledPixels;
}

void Grid::drawPolygon(Polygon &polygon) {
  lastStats_ = {}; // stats reset
  int pointCount = polygon.points.size();
  int filledPixels = 0; // stats

  if (pointCount < 3) {
    throw std::invalid_argument("Polygon requires at least 3 vertices");
  }

  for (int i = 0; i < pointCount; ++i) {
    Point &start = polygon.points[i];
    Point &end = polygon.points[(i + 1) % pointCount];
    drawLine(start, end);
    filledPixels += lastStats_.filledPixels; // stats
  }

  // stats
  lastStats_.filledPixels = filledPixels;
}

void Grid::lineFillPolygon(Polygon &polygon) {
  lastStats_ = {}; // stats reset
  int pointCount = polygon.points.size();
  int filledPixels = 0; // stats
  int steps = 0;        // stats

  if (pointCount < 3) {
    throw std::invalid_argument("Polygon requires at least 3 vertices");
  }

  // find intersections
  std::map<int, std::vector<double>> intersections;
  for (int i = 0; i < pointCount; ++i) {
    Point &start = polygon.points[i];
    Point &end = polygon.points[(i + 1) % pointCount];

    int startY = std::min(start.y, end.y);
    int endY = std::max(end.y, start.y);
    for (int y = startY; y < endY; y++) {
      double x = start.x +
                 (double)(y - start.y) * (end.x - start.x) / (end.y - start.y);
      intersections[y].push_back(x);
    }
  }

  // iterating map
  for (auto inter = intersections.begin(); inter != intersections.end();
       ++inter) {
    int y = inter->first;
    std::vector<double> value = inter->second;
    std::sort(value.begin(), value.end());

    // iterating intervals
    for (int i = 0; i < value.size() - 1; i += 2) {
      int startX = std::ceil(value[i]);
      int endX = std::floor(value[i + 1]);

      for (int x = startX; x <= endX; x++) {
        if (grid_[x][y] == emptyPixel) {
          filledPixels++;
        } // stats

        setPixel(Point(x, y), true);
        steps++; // stats
      }
    }
  }

  // stats
  lastStats_.filledPixels = filledPixels;
  lastStats_.steps = steps;
}

void Grid::floodFillPolygon(Point seed, bool isEightConnectivity) {
  lastStats_ = {}; // stats reset
  if (seed.x < 0 || seed.x >= width_ || seed.y < 0 || seed.y >= height_) {
    throw std::invalid_argument("Seed is out of bounds");
  }

  if (grid_[seed.x][seed.y] != emptyPixel) {
    return;
  }

  int filledPixels = 1; // Начальная затравка закрашивается сразу
  int steps = 0;        // stats

  const Point directions[] = {Point(0, 1),  Point(1, 0),  Point(0, -1),
                              Point(-1, 0), Point(1, 1),  Point(-1, 1),
                              Point(1, -1), Point(-1, -1)};
  int directionCount = isEightConnectivity ? 8 : 4;

  std::queue<Point> queuePoints;

  setPixel(seed, true);
  queuePoints.push(seed);

  while (!queuePoints.empty()) {
    Point currentPoint = queuePoints.front();
    queuePoints.pop();

    steps++; // stats

    // iterating neighbors
    for (int i = 0; i < directionCount; i++) {
      Point neighbor(currentPoint.x + directions[i].x,
                     currentPoint.y + directions[i].y);

      if (neighbor.x < 0 || neighbor.x >= width_ || neighbor.y < 0 ||
          neighbor.y >= height_) {
        continue;
      }

      if (grid_[neighbor.x][neighbor.y] != emptyPixel) {
        continue;
      }

      setPixel(neighbor, true);
      queuePoints.push(neighbor);

      filledPixels++; // stats
    }
  }

  // stats
  lastStats_.filledPixels = filledPixels;
  lastStats_.steps = steps;
}

std::vector<Point> Grid::rasterizeLineDDA(Point start, Point end) const {
  if (start.x >= width_ || start.x < 0) {
    throw std::invalid_argument("Start x coordinate is out of bounds");
  }
  if (start.y >= height_ || start.y < 0) {
    throw std::invalid_argument("Start y coordinate is out of bounds");
  }

  if (end.x >= width_ || end.x < 0) {
    throw std::invalid_argument("End x coordinate is out of bounds");
  }
  if (end.y >= height_ || end.y < 0) {
    throw std::invalid_argument("End y coordinate is out of bounds");
  }

  if (start.x == end.x && start.y == end.y) {
    return {start};
  }

  int dx = end.x - start.x;
  int dy = end.y - start.y;

  int steps = std::max(std::abs(dx), std::abs(dy));

  double xIncrement = static_cast<double>(dx) / steps;
  double yIncrement = static_cast<double>(dy) / steps;

  std::vector<Point> linePixels;

  for (int i = 0; i <= steps; i++) {
    linePixels.push_back(Point(std::lround(start.x + i * xIncrement),
                               std::lround(start.y + i * yIncrement)));
  }
  return linePixels;
}

void Grid::drawLineDDA(Point start, Point end) {
  lastStats_ = {}; // stats reset
  std::vector<Point> linePixels;
  linePixels = rasterizeLineDDA(start, end);

  int filledPixels = 0; // stats

  for (int i = 0; i < linePixels.size(); i++) {
    if (grid_[linePixels[i].x][linePixels[i].y] == emptyPixel) {
      filledPixels++;
    } // stats

    setPixel(linePixels[i], true);
  }

  // stats
  lastStats_.lineLength = linePixels.size();
  lastStats_.steps = linePixels.size() - 1;
  lastStats_.filledPixels = filledPixels;
}

void Grid::savePGM(const std::string &filename, int scale) const {
  if (scale <= 0) {
    throw std::invalid_argument("Scale must be greater than 0");
  }

  std::ofstream file(filename);

  if (!file) {
    throw std::runtime_error("Failed to open image file");
  }

  file << "P2\n";
  file << width_ * scale << " " << height_ * scale << "\n";
  file << "255\n";

  for (int y = height_ * scale - 1; y >= 0; y--) {
    for (int x = 0; x < width_ * scale; x++) {
      int sourceX = x / scale;
      int sourceY = y / scale;

      int brightness = grid_[sourceX][sourceY] == filledPixel ? 0 : 255;

      file << brightness << ' ';
    }

    file << '\n';
  }

  if (!file) {
    throw std::runtime_error("Failed to write image file");
  }
}