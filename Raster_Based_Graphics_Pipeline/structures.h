#include<iostream>
#include<math.h>
#include <iomanip>

#define pi (2*acos(0.0))
#define DIM 4

using namespace std;

/// --------------- Declarations ---------------- ///

class point;
class TransformationMatrix;

point TransformPoint(TransformationMatrix mat, point p);
point RodriguesFormula(point axis, point vector_to_be_rotated, double angle);
double round(double var);


/// --------------- Definitions ---------------- ///

class point
{
public:
    double x;
    double y;
    double z;

    point()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    point(double a, double b, double c)
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
    point operator+(const point v);                 // vector sum
    point operator-(const point v);                 // vector subtract
    point operator*(const double a);                // vector scale
    point operator^(const point v);                 // vector cross product
    double operator*(const point a);                // vector dot product
};

// vector normalize
void point::normalize()
{
    double scale = sqrt(x*x + y*y + z*z);

    x = x/scale;
    y = y/scale;
    z = z/scale;
}

// vector addition
point point::operator+(const point v)
{
    point result;
    result.x = x + v.x;
    result.y = y + v.y;
    result.z = z + v.z;

    return result;
}

// vector subtraction
point point::operator-(const point v)
{
    point result;
    result.x = x - v.x;
    result.y = y - v.y;
    result.z = z - v.z;

    return result;
}

// vector scale
point point::operator*(const double a)
{
    point result;
    result.x = x * a;
    result.y = y * a;
    result.z = z * a;

    return result;
}

// vector dot
double point::operator*(const point a)
{
    return x * a.x + y * a.y + z * a.z;
}

// vector cross
point point::operator^(const point v2)
{
    point product;

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

    void setMatrix(point a, point b, point c)
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
        point a(ax, ay, az);
        point i(1, 0 ,0);
        point j(0, 1, 0);
        point k(0, 0, 1);

        a.normalize();

        point c1 = RodriguesFormula(i, a, angle);
        point c2 = RodriguesFormula(j, a, angle);
        point c3 = RodriguesFormula(k, a, angle);

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


point RodriguesFormula(point axis, point vector_to_be_rotated, double angle)
{
    point result;

    result = axis * round(cos(angle * pi/180)) + vector_to_be_rotated * ((1 - cos(angle * pi/180)) * (axis * vector_to_be_rotated)) + (vector_to_be_rotated ^ axis) * sin(angle * pi/180);

    return result;
}


point TransformPoint(TransformationMatrix mat, point p)
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
    return (point(result_matrix[0][0], result_matrix[1][0], result_matrix[2][0]) * (1 / result_matrix[3][0]));   // balancing with weight
}
