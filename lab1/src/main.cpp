#include "grid.hpp"
#include <exception>
#include <cstdio>

int main() {
  try {
    Grid grid(20, 20);

    Point point = {.x = 5, .y = 5};

    grid.render();
    grid.setPixel(point, true);
    grid.render();
  } catch (std::exception &error) {
    printf("%s\n", error.what());
    return 1;
  }

  return 0;
}