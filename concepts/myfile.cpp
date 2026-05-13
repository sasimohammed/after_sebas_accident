// ID: 20231079  Name: sohila walid mohammed
// ID: 20231087  Name: seba ibraheem ezzat
// ID: 20230294  Name: klara sameh
// ID: 20230289  Name: katren nader

#include <iostream>

using namespace std;

struct ShapeVtable;

struct Shape
{
    ShapeVtable* vtable;
};

struct ShapeVtable
{
    double (*Area)(Shape*);
    void (*PrintInfo)(Shape*);
};

double GetArea(Shape* shape)
{
    return shape->vtable->Area(shape);
}

void PrintInfo(Shape* shape)
{
    shape->vtable->PrintInfo(shape);
}

// ================= Circle =================

struct Circle
{
    Shape parent;
    double radius;
};

double CircleArea(Shape* s)
{
    Circle* c;
    c = (Circle*) s;

    return 3.141592653589793 * c->radius * c->radius;
}

void CirclePrint(Shape* s)
{
    Circle* c;
    c = (Circle*) s;

    cout << "Circle radius = " << c->radius << endl;
    cout << "Circle Area = " << CircleArea(s) << endl;
}

ShapeVtable circle_vtable =
        {
                CircleArea,
                CirclePrint
        };

void CircleInitialize(Circle* c, double r)
{
    c->parent.vtable = &circle_vtable;
    c->radius = r;
}

// ================= Rectangle =================

struct Rectangle
{
    Shape parent;
    double width;
    double height;
};

double RectangleArea(Shape* s)
{
    Rectangle* r;
    r = (Rectangle*) s;

    return r->width * r->height;
}

void RectanglePrint(Shape* s)
{
    Rectangle* r;
    r = (Rectangle*) s;

    cout << "Rectangle width = " << r->width
         << ", height = " << r->height << endl;

    cout << "Rectangle Area = " << RectangleArea(s) << endl;
}

ShapeVtable rectangle_vtable =
        {
                RectangleArea,
                RectanglePrint
        };

void RectangleInitialize(Rectangle* r, double w, double h)
{
    r->parent.vtable = &rectangle_vtable;
    r->width = w;
    r->height = h;
}

// ================= Ellipse =================

struct Ellipse
{
    Shape parent;
    double a;
    double b;
};

double EllipseArea(Shape* s)
{
    Ellipse* e;
    e = (Ellipse*) s;

    return 3.141592653589793 * e->a * e->b;
}

void EllipsePrint(Shape* s)
{
    Ellipse* e;
    e = (Ellipse*) s;

    cout << "Ellipse a = " << e->a
         << ", b = " << e->b << endl;

    cout << "Ellipse Area = " << EllipseArea(s) << endl;
}

ShapeVtable ellipse_vtable =
        {
                EllipseArea,
                EllipsePrint
        };

void EllipseInitialize(Ellipse* e, double a, double b)
{
    e->parent.vtable = &ellipse_vtable;
    e->a = a;
    e->b = b;
}

// ================= Square =================

struct Square
{
    Shape parent;
    double side;
};

double SquareArea(Shape* s)
{
    Square* sq;
    sq = (Square*) s;

    return sq->side * sq->side;
}

void SquarePrint(Shape* s)
{
    Square* sq;
    sq = (Square*) s;

    cout << "Square side = " << sq->side << endl;
    cout << "Square Area = " << SquareArea(s) << endl;
}

ShapeVtable square_vtable =
        {
                SquareArea,
                SquarePrint
        };

void SquareInitialize(Square* s, double side)
{
    s->parent.vtable = &square_vtable;
    s->side = side;
}

// ================= Main =================

int main()
{
    Circle circle;
    CircleInitialize(&circle, 10);

    Rectangle rectangle;
    RectangleInitialize(&rectangle, 3, 5);

    Ellipse ellipse;
    EllipseInitialize(&ellipse, 10, 12);

    Square square;
    SquareInitialize(&square, 7);

    Shape* shapes[4];

    shapes[0] = (Shape*) &circle;
    shapes[1] = (Shape*) &rectangle;
    shapes[2] = (Shape*) &ellipse;
    shapes[3] = (Shape*) &square;

    double total_area;
    total_area = 0;

    int i;

    for(i = 0; i < 4; i++)
    {
        double d;

        d = GetArea(shapes[i]);

        total_area = total_area + d;

        PrintInfo(shapes[i]);
    }

    cout << total_area << endl;

    return 0;
}