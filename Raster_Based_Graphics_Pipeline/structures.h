#include<iostream>
#include<math.h>
#include <iomanip>

#define pi (2*acos(0.0))
#define DIM 4

using namespace std;

/// --------------- Declarations ---------------- ///

class Point;
class TransformationMatrix;

Point TransformPoint(TransformationMatrix mat, Point p);
Point RodriguesFormula(Point axis, Point vector_to_be_rotated, double angle);
double round(double var);


/// --------------- Definitions ---------------- ///

class Point
{
public:
    double x;
    double y;
    double z;

    Point()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    Point(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
    }

    void setPoint(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
    }

    void printPoint(ofstream &outFile)
    {
        outFile<<fixed<<setprecision(7)<<x<<" "<<y<<" "<<z<<endl;
    }

    void normalize();
    Point operator+(const Point v);                 // vector sum
    Point operator-(const Point v);                 // vector subtract
    Point operator*(const double a);                // vector scale
    Point operator^(const Point v);                 // vector cross product
    double operator*(const Point a);                // vector dot product
};

// vector normalize
void Point::normalize()
{
    double scale = sqrt(x*x + y*y + z*z);

    x = x/scale;
    y = y/scale;
    z = z/scale;
}

// vector addition
Point Point::operator+(const Point v)
{
    Point result;
    result.x = x + v.x;
    result.y = y + v.y;
    result.z = z + v.z;

    return result;
}

// vector subtraction
Point Point::operator-(const Point v)
{
    Point result;
    result.x = x - v.x;
    result.y = y - v.y;
    result.z = z - v.z;

    return result;
}

// vector scale
Point Point::operator*(const double a)
{
    Point result;
    result.x = x * a;
    result.y = y * a;
    result.z = z * a;

    return result;
}

// vector dot
double Point::operator*(const Point a)
{
    return x * a.x + y * a.y + z * a.z;
}

// vector cross
Point Point::operator^(const Point v2)
{
    Point product;

    product.x = y * v2.z - v2.y * z;
    product.y = v2.x * z - x * v2.z;
    product.z = x * v2.y - v2.x * y;

    return product;
}


class TransformationMatrix
{
public:
    double matrix[DIM][DIM];

    // initializing with 0s
    TransformationMatrix()
    {
        for(int i=0; i<DIM; i++)
        {
            for(int j=0; j<DIM; j++)
                matrix[i][j] = 0;
        }
    }

    // copying matrix
    TransformationMatrix(double mat[DIM][DIM])
    {
        for(int i=0; i<DIM; i++)
            for(int j=0; j<DIM; j++)
                matrix[i][j] = mat[i][j];
    }

    // multiplying two matrices
    TransformationMatrix operator*(const TransformationMatrix B)
    {
        TransformationMatrix result;

        for(int i = 0; i < DIM; i++)
        {
            for(int j = 0; j < DIM; j++)
            {
                result.matrix[i][j] = 0;

                for(int k = 0; k < DIM; k++)
                {
                    result.matrix[i][j] += matrix[i][k] * B.matrix[k][j];
                }
            }
        }
        return result;
    }

    void setMatrix(Point a, Point b, Point c)
    {
        matrix[0][0] = a.x;
        matrix[0][1] = a.y;
        matrix[0][2] = a.z;

        matrix[1][0] = b.x;
        matrix[1][1] = b.y;
        matrix[1][2] = b.z;

        matrix[2][0] = c.x;
        matrix[2][1] = c.y;
        matrix[2][2] = c.z;

        matrix[3][3] = 1;
    }

    void printMatrix()
    {
        for(int i=0; i<DIM; i++)
        {
            for(int j=0; j<DIM; j++)
                cout<<matrix[i][j]<<" ";
            cout<<endl;
        }
    }

    void IdentityMatrix()
    {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
        matrix[2][2] = 1;
        matrix[3][3] = 1;
    }

    void TranslationMatrix(double tx, double ty, double tz)
    {
        IdentityMatrix();

        matrix[0][3] = tx;
        matrix[1][3] = ty;
        matrix[2][3] = tz;
    }

    void ScaleMatrix(double sx, double sy, double sz)
    {
        matrix[0][0] = sx;
        matrix[1][1] = sy;
        matrix[2][2] = sz;
        matrix[3][3] = 1;
    }

    void RotationMatrix(double angle, double ax, double ay, double az)
    {
        Point a(ax, ay, az);
        Point i(1, 0 ,0);
        Point j(0, 1, 0);
        Point k(0, 0, 1);

        a.normalize();

        Point c1 = RodriguesFormula(i, a, angle);
        Point c2 = RodriguesFormula(j, a, angle);
        Point c3 = RodriguesFormula(k, a, angle);

        // Rotation Matrix Generation
        matrix[0][0] = c1.x;
        matrix[0][1] = c2.x;
        matrix[0][2] = c3.x;

        matrix[1][0] = c1.y;
        matrix[1][1] = c2.y;
        matrix[1][2] = c3.y;

        matrix[2][0] = c1.z;
        matrix[2][1] = c2.z;
        matrix[2][2] = c3.z;

        matrix[3][3] = 1;
    }

    void ProjectionMatrix(double fovY, double aspectRatio, double near, double far)
    {
        double fovX = fovY * aspectRatio;
        double t = near * tan(fovY/2 * pi/180);
        double r = near * tan(fovX/2 * pi/180);

        matrix[0][0] = near/r;
        matrix[1][1] = near/t;
        matrix[2][2] = -(far + near) / (far - near);
        matrix[2][3] = -(2 * far * near) / (far - near);
        matrix[3][2] = -1;
    }
};


double round(double var)
{
    double value = (int)(var * 1000000 + .5);
    return (double)value / 1000000;
}


Point RodriguesFormula(Point axis, Point vector_to_be_rotated, double angle)
{
    Point result;

    result = axis * round(cos(angle * pi/180)) + vector_to_be_rotated * ((1 - cos(angle * pi/180)) * (axis * vector_to_be_rotated)) + (vector_to_be_rotated ^ axis) * sin(angle * pi/180);

    return result;
}


Point TransformPoint(TransformationMatrix mat, Point p)
{
    double point_matrix[DIM][1];
    double result_matrix[DIM][1];

    point_matrix[0][0] = p.x;
    point_matrix[1][0] = p.y;
    point_matrix[2][0] = p.z;
    point_matrix[3][0] = 1;

    for(int i = 0; i < DIM; i++)
    {
        for(int j = 0; j < 1; j++)
        {
            result_matrix[i][j] = 0;

            for(int k = 0; k < DIM; k++)
            {
                result_matrix[i][j] += mat.matrix[i][k] * point_matrix[k][j];
            }
        }
    }

    /// weight = result_matrix[3][0];
    return (Point(result_matrix[0][0], result_matrix[1][0], result_matrix[2][0]) * (1 / result_matrix[3][0]));   // balancing with weight
}


// -------- for z_buffer ------------ //
struct Color
{
    double red;
    double green;
    double blue;
};


class Triangle
{
public:
    Point points[3];
    Color color;

    Triangle(Point point1, Point point2, Point point3)
    {
        points[0] = point1;
        points[1] = point2;
        points[2] = point3;

        color.red = rand() % 256;
        color.green = rand() % 256;
        color.blue = rand() % 256;
    }

    void printTriangle()
    {
        cout<<points[0].x<<" "<<points[0].y<<" "<<points[0].z<<endl;
        cout<<points[1].x<<" "<<points[1].y<<" "<<points[1].z<<endl;
        cout<<points[2].x<<" "<<points[2].y<<" "<<points[2].z<<endl;
        cout<<endl;
    }
};