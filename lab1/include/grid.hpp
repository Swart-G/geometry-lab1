#ifndef LAB1_GRID_H
#define LAB1_GRID_H
#include <vector>

typedef struct Point {
  int x;
  int y;
} Point;

typedef struct Polygon {
  std::vector<Point> points;
} Polygon;

class Grid {
public:
  Grid(int width, int height);
  ~Grid();

  int getWidth() const;
  int getHeight() const;

  void render() const;
  void setPixel(Point point, bool value);
  void clear();
  void drawLine(Point start, Point end);
  void drawPolygon(Polygon polygon);
  void lineFillPolygon(Polygon polygon);

private:
  std::vector<Point> rasterizeLine(Point start, Point end) const;
  int width_{};
  int height_{};
  std::vector<std::vector<char>> grid_;

  static constexpr char emptyPixel = '.';
  static constexpr char filledPixel = '*';
  static constexpr char framePixel = '#';
};

#endif // LAB1_GRID_H
