#include <vector>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <cmath>
#include "structures.h"
#include "bitmap_image.hpp"
#include "config.h"

#define INF 999999

using namespace std;

struct Color;
class Triangle;

// set in "read data"
int screen_width = 0;
int screen_height = 0;
double left_limit_x = -INF;
double right_limit_x = INF;
double bottom_limit_y = -INF;
double top_limit_y = INF;
double front_limit_z = -INF;
double rear_limit_z = INF;
vector<Triangle> triangles;

// set in "initialize buffers"
double dx, dy, top_y, bottom_y, left_x, right_x;
double **z_buffer;
Color **frame_buffer;


void read_data()
{
    /// ------ Read the "config.txt" file ------ ///
    string config_filepath = input_filepath + "config.txt";
    ifstream config_file;
    config_file.open(config_filepath);

    if(!config_file.is_open())
    {
        cout<<"Could not open Config file"<<endl;
        exit(1);
    }

    config_file>>screen_width>>screen_height;
    config_file>>left_limit_x;
    config_file>>bottom_limit_y;
    config_file>>front_limit_z>>rear_limit_z;

    right_limit_x = -left_limit_x;
    top_limit_y = -bottom_limit_y;

    config_file.close();


    /// ---- Read input information from the stage3.txt file ---- ///
    string stage3_filepath =  output_filepath + "stage3.txt";
    ifstream stage3_file;
    stage3_file.open(stage3_filepath);

    if(!stage3_file.is_open())
    {
        cout<<"Could not open Stage3 File"<<endl;
        exit(1);
    }

    double x1, y1, z1;
    double x2, y2, z2;
    double x3, y3, z3;
    while(true)
    {
        stage3_file>>x1>>y1>>z1;
        stage3_file>>x2>>y2>>z2;
        stage3_file>>x3>>y3>>z3;

        if(stage3_file.eof())
            break;

        Point p1(x1, y1, z1);
        Point p2(x2, y2, z2);
        Point p3(x3, y3, z3);

        Triangle t(p1, p2 ,p3);
        triangles.push_back(t);
    }

    stage3_file.close();
}


void initialize_buffers()
{
    dx = (right_limit_x - left_limit_x) / screen_width;
    dy = (top_limit_y - bottom_limit_y) / screen_height;

    top_y = top_limit_y - dy/2;
    bottom_y = bottom_limit_y + dy/2;
    left_x = left_limit_x + dx/2;
    right_x = right_limit_x - dx/2;


    // dynamically allocating 2d z_buffer
    // and initializing it with z_max
    z_buffer = new double*[screen_height];
    for(int i=0; i<screen_height; i++)
        z_buffer[i] = new double[screen_width];

    for(int i=0; i<screen_height; i++)
        for(int j=0; j<screen_width; j++)
            z_buffer[i][j] = rear_limit_z;


    // dynamically allocating a 2d frame_buffer
    // and initializing it with background color (black)
    frame_buffer = new Color*[screen_height];
    for(int i=0; i<screen_height; i++)
        frame_buffer[i] = new Color[screen_width];

    for(int i=0; i<screen_height; i++)
        for(int j=0; j<screen_width; j++)
        {
            frame_buffer[i][j].red = 0;
            frame_buffer[i][j].green = 0;
            frame_buffer[i][j].blue = 0;
        }
}

void apply_procedure()
{
    int top_scanline;
    int bottom_scanline;

    for(int i=0; i<triangles.size(); i++)
    {
        /// --------- finding top and bottom scanline ----------- ///
        // find the max and min y co-ordinates of the corners of the triangle
        // if max_y exceeds topY or if min_y exceeds bottomY
        // clip that
        double max_y = max(triangles[i].points[0].y, max(triangles[i].points[1].y, triangles[i].points[2].y));
        double min_y = min(triangles[i].points[0].y, min(triangles[i].points[1].y, triangles[i].points[2].y));

        // ---------- clipping of y co-ordinates ---------- //
        if(max_y >= top_y)
        {
            top_scanline = 0;
        }
        else
        {
            top_scanline = round((top_y - max_y)/dy);
        }

        if(min_y <= bottom_y)
        {
            bottom_scanline = screen_height - 1;
        }
        else
        {
            bottom_scanline = (screen_height - 1) - (round((min_y - bottom_y)/dy));
        }

        /// -------- scanning with scan lines -------- ///
        for(int row = top_scanline; row <= bottom_scanline; row++)
        {
            double ys = top_y - row * dy;

            Point intersecting_points[3];
            intersecting_points[0].x = intersecting_points[1].x = intersecting_points[2].x = INF;
            intersecting_points[0].y = intersecting_points[1].y = intersecting_points[2].y = ys;

            // intersecting_points[0] is of triangle_points 0 and 1
            // intersecting_points[1] is of triangle_points 1 and 2
            // intersecting_points[2] is of triangle_points 2 and 0

            Point p1, p2, p3;
            p1 = triangles[i].points[0];
            p2 = triangles[i].points[1];
            p3 = triangles[i].points[2];

            // determine x co-ordinates of intersection points
            // x = x1 + (ys - y1)/(y2 - y1) * (x2 - x1)
            if(p1.y != p2.y)
            {
                intersecting_points[0].x = p1.x + (ys - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
            }
            if(p2.y != p3.y)
            {
                intersecting_points[1].x = p2.x + (ys - p2.y) * (p3.x - p2.x) / (p3.y - p2.y);
            }
            if(p3.y != p1.y)
            {
                intersecting_points[2].x = p3.x + (ys - p3.y) * (p1.x - p3.x) / (p1.y - p3.y);
            }

            // filter out the intersecting point that lies in the extended part
            if(intersecting_points[0].x != INF)
            {
                if(intersecting_points[0].x < min(p1.x, p2.x) || intersecting_points[0].x > max(p1.x, p2.x))
                    intersecting_points[0].x = INF;
            }
            if(intersecting_points[1].x != INF)
            {
                if(intersecting_points[1].x < min(p2.x, p3.x) || intersecting_points[1].x > max(p2.x, p3.x))
                    intersecting_points[1].x = INF;
            }
            if(intersecting_points[2].x != INF)
            {
                if(intersecting_points[2].x < min(p3.x, p1.x) || intersecting_points[2].x > max(p3.x, p1.x))
                    intersecting_points[2].x = INF;
            }



            // determine which point is left and which one is right
            int left_idx = -1;
            int right_idx = -1;
            pair<Point, Point> left_intersecting_points_ends;
            pair<Point, Point> right_intersecting_points_ends;

            double max_x, min_x;

            for(int j=0; j<3; j++)
            {
                if(left_idx == -1 && right_idx == -1)
                {
                    if(intersecting_points[j].x != INF)
                    {
                        left_idx = right_idx = j;
                        max_x = min_x = intersecting_points[j].x;
                    }
                }
                else
                {
                    if(intersecting_points[j].x != INF)
                    {
                        if(intersecting_points[j].x > max_x)
                        {
                            right_idx = j;
                            max_x = intersecting_points[j].x;
                        }
                        if(intersecting_points[j].x < min_x)
                        {
                            left_idx = j;
                            min_x = intersecting_points[j].x;
                        }
                    }
                }
            }


            // finding the triangle vertex of the intersection points
            if(right_idx == 0)
            {
                right_intersecting_points_ends.first = p1;
                right_intersecting_points_ends.second = p2;
            }
            else if(right_idx == 1)
            {
                right_intersecting_points_ends.first = p2;
                right_intersecting_points_ends.second = p3;
            }
            else
            {
                right_intersecting_points_ends.first = p3;
                right_intersecting_points_ends.second = p1;
            }

            if(left_idx == 0)
            {
                left_intersecting_points_ends.first = p1;
                left_intersecting_points_ends.second = p2;
            }
            else if(left_idx == 1)
            {
                left_intersecting_points_ends.first = p2;
                left_intersecting_points_ends.second = p3;
            }
            else
            {
                left_intersecting_points_ends.first = p3;
                left_intersecting_points_ends.second = p1;
            }



            // ---------- clipping of x co-ordinates ---------- //
            int left_intersecting_col;
            int right_intersecting_col;
            if( min_x <= left_x)
            {
                left_intersecting_col = 0;
            }
            else
            {
                left_intersecting_col = round((min_x - left_x)/dx);
            }

            if( max_x >= right_x )
            {
                right_intersecting_col = screen_width - 1;
            }
            else
            {
                right_intersecting_col = (screen_width - 1) - round((right_x - max_x)/dx);
            }

            // -------- determine za and zb -------- //
            // za = z1 + (ys - y1)/(y2 - y1) * (z2 - z1)

            p1 = left_intersecting_points_ends.first;
            p2 = left_intersecting_points_ends.second;

            double za;
            za = p1.z + (ys - p1.y) * (p2.z - p1.z) / (p2.y - p1.y);

            // zb = z1 + (ys - y1)/(y2 - y1) * (z2 - z1)
            p1 = right_intersecting_points_ends.first;
            p2 = right_intersecting_points_ends.second;

            double zb;
            zb = p1.z + (ys - p1.y) * (p2.z - p1.z) / (p2.y - p1.y);

            // -------- determine zp --------- //
            // zp = zb - (zb - za) * (xb - xp) / (xb - xa)
            double zp;
            double constant = dx * (zb - za) / (intersecting_points[right_idx].x - intersecting_points[left_idx].x);

            for(int col = left_intersecting_col; col <= right_intersecting_col; col++)
            {
                // if on first iteration
                if(col == left_intersecting_col)
                {
                    zp = zb - (zb - za) * (intersecting_points[right_idx].x - (left_x + left_intersecting_col * dx)) / (intersecting_points[right_idx].x - intersecting_points[left_idx].x);
                }
                else
                {
                    zp = zp + constant;
                }

                if((zp > front_limit_z) && (zp < z_buffer[row][col]))
                {
                    z_buffer[row][col] = zp;
                    frame_buffer[row][col] = triangles[i].color;
                }
            }
        }
    }
}


void save()
{
    // generating out.bmp
    bitmap_image output_image(screen_width, screen_height);

    for(int i=0; i<screen_height; i++)
    {
        for(int j=0; j<screen_width; j++)
        {
            output_image.set_pixel(j, i, frame_buffer[i][j].red, frame_buffer[i][j].green, frame_buffer[i][j].blue);
        }
    }

    output_image.save_image(output_filepath + "out.bmp");

    // generating z_buffer.txt
    ofstream z_buffer_output(output_filepath + "z_buffer.txt");
    for(int i=0; i<screen_height; i++)
    {
        for(int j=0; j<screen_width; j++)
        {
            if(z_buffer[i][j] < rear_limit_z)
            {
                z_buffer_output<<z_buffer[i][j]<<"\t";
            }
        }
        z_buffer_output<<endl;
    }
    z_buffer_output.close();
}


void free_memory()
{
    for(int i=0; i<screen_height; i++)
    {
        delete[] z_buffer[i];
        delete[] frame_buffer[i];
    }
    delete[] z_buffer;
    delete[] frame_buffer;
}




void run_zBuffer_algorithm()
{
    read_data();
    initialize_buffers();
    apply_procedure();
    save();
    free_memory();
}
