#include "file_system.h"
#include <fstream>

void FileSystem::save(const CoreData& core, std::string filename) {
    std::ofstream out(filename);

    for (auto& s : core.getAllShapes()) {
        out << s.type << " "
            << s.color << " ";

        for (auto& p : s.points)
            out << p.x << " " << p.y << " ";

        out << s.radius << " "
            << s.width << " "
            << s.height << "\n";
    }
}

void FileSystem::load(CoreData& core, std::string filename) {
    std::ifstream in(filename);

    core.clearAll();

    int type;
    std::string color;

    while (in >> type >> color) {
        Shape s;
        s.type = (ShapeType)type;
        s.color = color;

        int x, y;
        std::vector<Point> pts;

        for (int i = 0; i < 2; i++) {
            if (in >> x >> y)
                pts.push_back({ x, y });
        }

        in >> s.radius >> s.width >> s.height;

        s.points = pts;
        core.addShape(s);
    }
}