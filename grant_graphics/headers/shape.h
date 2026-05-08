#pragma once
#include <vector>
#include <string>
using namespace std;

enum ShapeType {
    LINE,
    CIRCLE,
    ELLIPSE,
    RECTANGLE,
    SQUARE
};

struct Point {
    int x, y;
};

struct Shape {
    ShapeType type;

    vector<Point> points;   

    string color;

   
    int radius = 0;
    int width = 0;
    int height = 0;
};