#include "grid.hpp"
#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#define DEFAULT_WIDTH 40
#define DEFAULT_HEIGHT 20

void runCase0() {
  int algorithm;
  printf("1 — Брезенхэм\n"
         "2 — DDA\n"
         "Выберите алгоритм: ");
  if (!(std::cin >> algorithm) || (algorithm != 1 && algorithm != 2)) {
    printf("Некорректный выбор алгоритма\n");
    return;
  }

  Point start;
  Point end;
  printf("Введите начало отрезка (x y): ");
  if (!(std::cin >> start.x >> start.y)) {
    printf("Некорректные координаты\n");
    return;
  }

  printf("Введите конец отрезка (x y): ");
  if (!(std::cin >> end.x >> end.y)) {
    printf("Некорректные координаты\n");
    return;
  }

  Grid grid(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  if (algorithm == 1) {
    grid.drawLine(start, end);
  } else {
    grid.drawLineDDA(start, end);
  }
  grid.render();
}

void runLineComparison(Point start, Point end, const std::string &filePrefix,
                       int lineNumber) { // 1
  printf("___________________________________________________________\n");
  Grid grid1(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  Grid grid2(DEFAULT_WIDTH, DEFAULT_HEIGHT);

  grid1.drawLine(start, end);
  grid2.drawLineDDA(start, end);

  printf("Bresenham -- Steps: %d; Filled Pixels: %d; Line Lenght: %d\n",
         grid1.getSteps(), grid1.getFilledPixels(), grid1.getLineLength());
  grid1.render();

  printf("DDA -- Steps: %d; Filled Pixels: %d; Line Lenght: %d\n",
         grid2.getSteps(), grid2.getFilledPixels(), grid2.getLineLength());
  grid2.render();

  if (!filePrefix.empty()) {
    const std::string linePrefix =
        filePrefix + "_line" + std::to_string(lineNumber);
    grid1.savePGM(linePrefix + "_bresenham.pgm");
    grid2.savePGM(linePrefix + "_dda.pgm");
  }
}

void runScanlineTest(Polygon polygon, const std::string &filePrefix) { // 2
  printf("___________________________________________________________\n");
  Grid grid(DEFAULT_WIDTH, DEFAULT_HEIGHT);

  grid.drawPolygon(polygon);
  grid.render();
  if (!filePrefix.empty()) {
    grid.savePGM(filePrefix + "_outline.pgm");
  }

  grid.lineFillPolygon(polygon);
  grid.render();

  if (!filePrefix.empty()) {
    grid.savePGM(filePrefix + "_scanline.pgm");
  }
}

void runFloodFillTest(Polygon square, Point seed, bool isEightConnectivity,
                      const std::string &filePrefix) { // 3-4
  printf("___________________________________________________________\n");
  Grid grid(DEFAULT_WIDTH, DEFAULT_HEIGHT);

  grid.drawPolygon(square);
  grid.render();
  if (!filePrefix.empty() && !isEightConnectivity) {
    grid.savePGM(filePrefix + "_outline.pgm");
  }

  printf("Flood fill (%d-связность)\n", isEightConnectivity ? 8 : 4);
  grid.floodFillPolygon(seed, isEightConnectivity);
  grid.render();

  if (!filePrefix.empty()) {
    grid.savePGM(filePrefix + (isEightConnectivity ? "_floodfill_8.pgm"
                                                   : "_floodfill_4.pgm"));
  }
}

void runCase1(const std::string &filePrefix = {}) {
  // 1. Первый октант
  runLineComparison(Point(0, 0), Point(5, 2), filePrefix, 1);

  // 2. Отрицательный наклон
  runLineComparison(Point(0, 2), Point(5, 0), filePrefix, 2);

  // 3. Вертикальный отрезок
  runLineComparison(Point(3, 0), Point(3, 5), filePrefix, 3);

  // 4. Горизонтальный отрезок
  runLineComparison(Point(0, 3), Point(5, 3), filePrefix, 4);

  // 5. Обратная диагональ
  runLineComparison(Point(5, 0), Point(0, 5), filePrefix, 5);

  // 6. Отрицательный наклон, четыре расхождения
  runLineComparison(Point(0, 4), Point(8, 0), filePrefix, 6);

  // 7. Длинный отрезок, шесть расхождений
  runLineComparison(Point(0, 6), Point(12, 0), filePrefix, 7);

  // 8. Обратное направление
  runLineComparison(Point(8, 4), Point(0, 0), filePrefix, 8);

  // 9. Крутой положительный наклон
  runLineComparison(Point(8, 0), Point(4, 8), filePrefix, 9);
}

void runCase2(const std::string &filePrefix = {}) {
  // 10. Треугольник с заливкой (scanline)
  Polygon triangle;
  triangle.points = {Point(1, 1), Point(8, 1), Point(4, 6)};

  runScanlineTest(triangle, filePrefix);
}

void runCase3(const std::string &filePrefix = {}) {
  // 11. Квадрат с заливкой (floodfill) с 4- и 8-связной заливкой
  Polygon square;
  square.points = {Point(2, 2), Point(6, 2), Point(6, 6), Point(2, 6)};
  Point seed(4, 4);

  runFloodFillTest(square, seed, false, filePrefix);
  runFloodFillTest(square, seed, true, filePrefix);
}

void runCase4(const std::string &filePrefix = {}) {
  // 12. Лесенка (ромб) (floodfill) с 4- и 8-связной заливкой
  Polygon diamond;
  diamond.points = {Point(10, 2), Point(16, 8), Point(10, 14), Point(4, 8)};
  Point seed(10, 8);

  runFloodFillTest(diamond, seed, false, filePrefix);
  runFloodFillTest(diamond, seed, true, filePrefix);
}

void runCase5() {
  std::filesystem::create_directories("case1");
  std::filesystem::create_directories("case2");
  std::filesystem::create_directories("case3");
  std::filesystem::create_directories("case4");

  runCase1("case1/case1");
  runCase2("case2/case2");
  runCase3("case3/case3");
  runCase4("case4/case4");
}

int main() {
  int choice;

  printf(
      "0 — Построение отрезка по введённым координатам.\n"
      "1 — Сравнение алгоритмов Брезенхэма и DDA на девяти отрезках.\n"
      "2 — Построчная заливка треугольника.\n"
      "3 — Заливка квадрата с затравкой при 4- и 8-связности.\n"
      "4 — Заливка ромба с затравкой и демонстрация протечки при 8-связности.\n"
      "5 — Запуск тестов 1–4 и сохранение результатов в файлы PGM.\n");
  printf("Выберите тест (0–5): ");

  if (scanf("%d", &choice) != 1) {
    printf("Некорректный ввод\n");
    return 1;
  }

  try {
    switch (choice) {
    case 0:
      runCase0();
      break;

    case 1:
      runCase1();
      break;

    case 2:
      runCase2();
      break;

    case 3:
      runCase3();
      break;

    case 4:
      runCase4();
      break;

    case 5:
      runCase5();
      break;

    default:
      printf("Некорректный ввод\n");
    }

  } catch (std::exception &error) {
    printf("%s\n", error.what());
    return 1;
  }

  return 0;
}
