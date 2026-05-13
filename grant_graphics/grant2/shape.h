#pragma once
#include <vector>
#include <string>
using namespace std;

// في core_data.h
enum ShapeType {
    LINE,
    CIRCLE,
    ELLIPSE,
    CURVE,
    POINT_SHAPE,
    FILLED_SHAPE,
    FLOOD_FILL_SHAPE, 
    SMILEY_HAPPY,
    SMILEY_SAD,
    POINT_DOT,

};

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Shape {
    ShapeType type;
    vector<Point> points;
    string color;

    int radius = 0;
    int width = 0;
    int height = 0;
    int algorithm = 0;
};