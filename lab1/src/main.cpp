#include "grid.hpp"
#include <exception>
#include <cstdio>

int main() {
  try {
    Grid grid(80, 20);

    Polygon polygon;
    polygon.points = {Point(10, 5), Point(40, 18), Point(75, 10)};

    grid.drawPolygon(polygon);
    grid.render();
    grid.lineFillPolygon(polygon);
    grid.render();
  } catch (std::exception &error) {
    printf("%s\n", error.what());
    return 1;
  }

  return 0;
}