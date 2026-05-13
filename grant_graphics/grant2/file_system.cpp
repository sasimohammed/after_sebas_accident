#include "file_system.h"
#include <fstream>
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")

using namespace std;

// ================= SAVE DIALOG =================
static string OpenSaveDialog()
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Shape Files\0*.shp\0All Files\0*.*\0";
    ofn.lpstrDefExt = "shp";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameA(&ofn))
        return string(szFile);

    return "";
}

// ================= LOAD DIALOG =================
static string OpenLoadDialog()
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Shape Files\0*.shp\0All Files\0*.*\0";
    ofn.lpstrDefExt = "shp";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
        return string(szFile);

    return "";
}

// ================= SAVE =================
void FileSystem::save(const CoreData& core)
{
    string filename = OpenSaveDialog();
    if (filename.empty())
        return;

    ofstream out(filename);
    if (!out.is_open())
        return;

    const auto& shapes = core.getAllShapes();
    out << shapes.size() << "\n";

    for (const auto& s : shapes)
    {
        out << (int)s.type << "\n";
        out << s.color << "\n";

        out << s.points.size() << "\n";
        for (const auto& p : s.points)
            out << p.x << " " << p.y << "\n";

        out << s.radius << "\n";
        out << s.width << "\n";
        out << s.height << "\n";
    }

    out.close();
}

// ================= LOAD =================
void FileSystem::load(CoreData& core)
{
    string filename = OpenLoadDialog();
    if (filename.empty())
        return;

    ifstream in(filename);
    if (!in.is_open())
        return;

    core.clearAll();

    int shapeCount;
    in >> shapeCount;
    in.ignore();

    for (int i = 0; i < shapeCount; i++)
    {
        Shape s;
        int type;

        in >> type;
        s.type = (ShapeType)type;
        in.ignore();

        getline(in, s.color);

        int pointCount;
        in >> pointCount;
        in.ignore();

        for (int j = 0; j < pointCount; j++)
        {
            Point p;
            in >> p.x >> p.y;
            in.ignore();
            s.points.push_back(p);
        }

        in >> s.radius;
        in >> s.width;
        in >> s.height;
        in.ignore();

        core.addShape(s);
    }

    in.close();
}

// ================= CLEAR SCREEN =================
void FileSystem::clearScreen(HWND hwnd, CoreData& core)
{
    core.clearAll();
    InvalidateRect(hwnd, NULL, TRUE);
}