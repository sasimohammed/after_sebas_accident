#pragma once
#include <string>
using namespace std;

class Preferences {
private:
    string backgroundColor = "WHITE";
    string currentColor = "BLACK";

public:
    void setBackground(string color);
    string getBackground();

    void setCurrentColor(string color);
    string getCurrentColor();
};