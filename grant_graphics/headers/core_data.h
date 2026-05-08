#pragma once
#include "shape.h"
#include <vector>

class CoreData {
private:
    vector<Shape> shapes;

public:
    void addShape(const Shape& s);
    void removeLast();
    void clearAll();

 
    const vector<Shape>& getAllShapes() const;
};