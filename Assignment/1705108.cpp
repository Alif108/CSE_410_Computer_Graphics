#include <fstream>
#include <iostream>
#include "algorithms.h"

using namespace std;

int screen_width = 0;
int screen_height = 0;
int number_of_lines = 0;

int main()
{
    ifstream input;
    input.open("input.txt");

    if(!input.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }

    input >> screen_width >> screen_height;
    input >> number_of_lines;

    struct point points[2 * number_of_lines];   // 2 points per line

    for(int i=0; i < number_of_lines; i++) {
        input >> points[2*i].x >> points[2*i].y;
        input >> points[2*i+1].x >> points[2*i+1].y;
    }

    input.close();

    bitmap_image output(screen_width, screen_height);
    for(int i=0; i<number_of_lines; i++) {
        midPointLine(points[2*i].x, points[2*i].y, points[2*i+1].x, points[2*i+1].y, output);
    }
}