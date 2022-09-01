#include <cstdlib>
#include <time.h>
#include <cmath>

#include "bitmap_image.hpp"
#include "structures.h"

// struct color random_color = {255, 255, 0};

void IntensifyPixel(int x, int y, double distance) {
//   double intensity = Filter(Math.round(Math.abs(distance)));
//   WritePixel(x,y,intensity);
 }

void midPointLine(int x0, int y0, int x1, int y1, bitmap_image& output)
{
    srand(time(0));
    struct color random_color = {rand() % 256, rand() % 256, rand() % 256};

    int dx = x1 - x0;
    int dy = y1 - y0;
    int d = 2 * dy - dx;
    int incrE = 2 * dy;
    int incrNE = 2 * (dy - dx);

    // code added for antialiasing
      int two_v_dx = 0;                                   
      double invDenom = 1.0/(2.0 * sqrt(dx*dx + dy*dy)); 
      double two_dx_invDenom = 2.0 * dx * invDenom;           
    //

    int x = x0;
    int y = y0;

    // code added for antialiasing
      IntensifyPixel(x, y, 0);                       // start pixel
      IntensifyPixel(x, y+1, two_dx_invDenom);       // neighbor above
      IntensifyPixel(x, y-1, two_dx_invDenom);       // neighbor below
    //

    // output.set_pixel(x, y, random_color.r, random_color.g, random_color.b);

    while (x < x1)
    {
        if (d <= 0)
        {
            // code added for antialiasing
                two_v_dx = d+dx;              
            //
            d += incrE;
            x++;
        }
        else
        {
            // code added for antialiasing
                two_v_dx = d-dx;              
            //
            d += incrNE;
            x++;
            y++;
        }
        // output.set_pixel(x, y, random_color.r, random_color.g, random_color.b);
        
        // code added for antialiasing
            IntensifyPixel(x, y, two_v_dx * invDenom);                    
            IntensifyPixel(x, y+1, two_dx_invDenom - two_v_dx * invDenom);
            IntensifyPixel(x, y-1, two_dx_invDenom + two_v_dx * invDenom);
        //
    }

    output.save_image("output.bmp");
}