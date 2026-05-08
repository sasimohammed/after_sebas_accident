#include "preferences.h"

void Preferences::setBackground(string color) {
    backgroundColor = color;
}

string Preferences::getBackground() {
    return backgroundColor;
}

void Preferences::setCurrentColor(string color) {
    currentColor = color;
}

string Preferences::getCurrentColor() {
    return currentColor;
}