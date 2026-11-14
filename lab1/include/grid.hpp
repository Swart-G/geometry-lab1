#ifndef LAB1_GRID_H
#define LAB1_GRID_H

class Grid {
    public:
    Grid(int width, int height);
    -Grid();

    int getWidth() const;
    int getHeight() const;

    void render() const;
    bool setPixel(int x, int y, bool value);
    void clear();

    private:
    int width_{};
    int height_{};
};

#endif //LAB1_GRID_H
