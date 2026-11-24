#ifndef LAB1_GRID_H
#define LAB1_GRID_H
#include <string>
#include <vector>

typedef struct Point {
  int x;
  int y;
} Point;

typedef struct Polygon {
  std::vector<Point> points;
} Polygon;

typedef struct OperationStats {
  int filledPixels = 0;
  int lineLength = 0;
  int steps = 0;
} OperationStats;

class Grid {
public:
  Grid(int width, int height);
  ~Grid();

  int getWidth() const;
  int getHeight() const;
  int getSteps() const;
  int getLineLength() const;
  int getFilledPixels() const;

  void render() const;
  void setPixel(Point point, bool value);
  void clear();
  void drawLine(Point start, Point end);
  void drawPolygon(Polygon &polygon);
  void lineFillPolygon(Polygon &polygon);
  void floodFillPolygon(Point seed, bool isEightConnectivity = false);
  void drawLineDDA(Point start, Point end);
  void savePGM(const std::string &filename, int scale = 10) const;

private:
  std::vector<Point> rasterizeLine(Point start, Point end) const;
  std::vector<Point> rasterizeLineDDA(Point start, Point end) const;
  int width_{};
  int height_{};
  std::vector<std::vector<char>> grid_;
  OperationStats lastStats_;

  static constexpr char emptyPixel = '.';
  static constexpr char filledPixel = '*';
  static constexpr char framePixel = '#';
};

#endif // LAB1_GRID_H
