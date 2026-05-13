#include "core_data.h"

void CoreData::addShape(const Shape& s) {
    shapes.push_back(s);
}

void CoreData::removeLast() {
    if (!shapes.empty())
        shapes.pop_back();
}

void CoreData::clearAll() {
    shapes.clear();
}

const vector<Shape>& CoreData::getAllShapes() const {
    return shapes;
}