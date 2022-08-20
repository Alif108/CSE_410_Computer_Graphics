#include <windows.h>
#include <GL/glut.h>
#include <string>
#include <algorithm>
#include <fstream>

#define pi (2*acos(0.0))
#define INF 1e8

#define AMB 0
#define DIFF 1
#define SPEC 2
#define REC_REFFLECTION 3

using namespace std;

/// --------------------- Data Structures and necessary functions ---------------------- ///


class Color{
public:
    double red, green, blue;

    Color()
    {
        red = 0;
        green = 0;
        blue = 0;
    }

    Color(double r, double g, double b)
    {
        red = r;
        green = g;
        blue = b;
    }

    void setColor(double r, double g, double b)
    {
        red = r;
        green = g;
        blue = b;
    }

    void clipColor()
    {
        if(red > 1)
            red = 1;
        if(green > 1)
            green = 1;
        if(blue > 1)
            blue = 1;

        if(red < 0)
            red = 0;
        if(green < 0)
            green = 0;
        if(blue < 0)
            blue = 0;
    }

    ~Color()
    {
        red = 0;
        green = 0;
        blue = 0;
    }

    Color operator+(const Color c);                     // add two colors
    Color operator*(const double a);                    // multiply color by a scalar
    Color operator*(Color c);                           // multiply color by another color
};

Color Color::operator+(const Color c)
{
    Color result;
    result.red = red + c.red;
    result.green = green + c.green;
    result.blue = blue + c.blue;

    return result;
}

Color Color::operator*(const double a)
{
    Color result;
    result.red = red * a;
    result.green = green * a;
    result.blue = blue * a;

    return result;
}

Color Color::operator*(Color c)
{
    Color result;
    result.red = red * c.red;
    result.green = green * c.green;
    result.blue = blue * c.blue;

    return result;
}



class Vector3D{
public:
    double x;
    double y;
    double z;

    Vector3D()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3D(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
    }

    void setVector(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
    }

    void normalize();                                       // vector normalize
    double euclideanDistance(Vector3D v);                   // euclidean distance between two vectors
    Vector3D operator+(const Vector3D v);                   // vector sum
    Vector3D operator-(const Vector3D v);                   // vector subtract
    Vector3D operator*(const double a);                     // vector scale
    Vector3D operator^(const Vector3D v);                   // vector cross product
    double operator*(const Vector3D a);                     // vector dot product
};

void Vector3D::normalize()
{
    double magnitutde = sqrt(x*x + y*y + z*z);

    x = x/magnitutde;
    y = y/magnitutde;
    z = z/magnitutde;
}

double Vector3D::euclideanDistance(Vector3D v)
{
    return sqrt((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z));
}

Vector3D Vector3D::operator+(const Vector3D v)
{
    Vector3D result;
    result.x = x + v.x;
    result.y = y + v.y;
    result.z = z + v.z;

    return result;
}

Vector3D Vector3D::operator-(const Vector3D v)
{
    Vector3D result;
    result.x = x - v.x;
    result.y = y - v.y;
    result.z = z - v.z;

    return result;
}

Vector3D Vector3D::operator*(const double a)
{
    Vector3D result;
    result.x = x * a;
    result.y = y * a;
    result.z = z * a;

    return result;
}

Vector3D Vector3D::operator^(const Vector3D v2)
{
    Vector3D product;

    product.x = y * v2.z - v2.y * z;
    product.y = v2.x * z - x * v2.z;
    product.z = x * v2.y - v2.x * y;

    return product;
}

double Vector3D::operator*(const Vector3D a)
{
    return x * a.x + y * a.y + z * a.z;
}


class Ray{
public:
    Vector3D Ro;        // origin of the ray
    Vector3D Rd;        // direction of the ray

    Ray(Vector3D start, Vector3D direction)
    {
        this->Ro = start;
        this->Rd = direction;
        this->Rd.normalize();
    }

    ~Ray()
    {
        Ro.x = 0;
        Ro.y = 0;
        Ro.z = 0;
        Rd.x = 0;
        Rd.y = 0;
        Rd.z = 0;
    }
};


class PointLight
{
    double radius;
    int slices;
    int stacks;

public:
    Vector3D light_pos;
    Color color;

    PointLight(Vector3D light_pos, double r, double g, double b)
    {
        this->light_pos = light_pos;
        color.setColor(r, g, b);

        radius = 1;
        slices = 12;
        stacks = 4;
    }

    void draw()
    {
        Vector3D points[stacks+1][slices+1];
        int i,j;
        double h,r;
        //generate points
        for(i=0;i<=stacks;i++)
        {
            h=radius*sin(((double)i/(double)stacks)*(pi/2));
            r=radius*cos(((double)i/(double)stacks)*(pi/2));
            for(j=0;j<=slices;j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }

        /* drawing quads using generated points */
        glColor3f(color.red, color.green, color.blue);

        for(int i=0; i<stacks; i++) {
            for(int j=0; j<slices; j++) {
                glBegin(GL_QUADS);
                {
                    /* upper hemisphere */
                    glVertex3f(light_pos.x + points[i][j].x, light_pos.y + points[i][j].y, light_pos.z + points[i][j].z);
                    glVertex3f(light_pos.x + points[i][j+1].x, light_pos.y + points[i][j+1].y, light_pos.z + points[i][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j+1].x, light_pos.y + points[i+1][j+1].y, light_pos.z + points[i+1][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j].x, light_pos.y + points[i+1][j].y, light_pos.z + points[i+1][j].z);

                    /* lower hemisphere */
                    glVertex3f(light_pos.x + points[i][j].x, light_pos.y + points[i][j].y, light_pos.z - points[i][j].z);
                    glVertex3f(light_pos.x + points[i][j+1].x, light_pos.y + points[i][j+1].y, light_pos.z - points[i][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j+1].x, light_pos.y + points[i+1][j+1].y, light_pos.z - points[i+1][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j].x, light_pos.y + points[i+1][j].y, light_pos.z - points[i+1][j].z);
                }
                glEnd();
            }
        }
    }

    ~PointLight()
    {
        light_pos.x = 0;
        light_pos.y = 0;
        light_pos.z = 0;
        color.red = 0;
        color.green = 0;
        color.blue = 0;
    }
};

class SpotLight
{
    double radius;
    int slices;
    int stacks;

public:
    Vector3D light_pos;
    Vector3D light_dir;
    Color color;
    double cutoff_angle;

    SpotLight(Vector3D light_pos, Vector3D light_dir , double r, double g, double b, double cutoff_angle)
    {
        this->light_pos = light_pos;
        this->light_dir = light_dir;
        color.setColor(r, g, b);
        this->cutoff_angle = cutoff_angle;
        radius = 1;
        slices = 12;
        stacks = 4;
    }
    

    void draw()
    {
        Vector3D points[stacks+1][slices+1];
        int i,j;
        double h,r;
        //generate points
        for(i=0;i<=stacks;i++)
        {
            h=radius*sin(((double)i/(double)stacks)*(pi/2));
            r=radius*cos(((double)i/(double)stacks)*(pi/2));
            for(j=0;j<=slices;j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }

        /* drawing quads using generated points */
        glColor3f(color.red, color.green, color.blue);

        for(int i=0; i<stacks; i++) {
            for(int j=0; j<slices; j++) {
                glBegin(GL_QUADS);
                {
                    /* upper hemisphere */
                    glVertex3f(light_pos.x + points[i][j].x, light_pos.y + points[i][j].y, light_pos.z + points[i][j].z);
                    glVertex3f(light_pos.x + points[i][j+1].x, light_pos.y + points[i][j+1].y, light_pos.z + points[i][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j+1].x, light_pos.y + points[i+1][j+1].y, light_pos.z + points[i+1][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j].x, light_pos.y + points[i+1][j].y, light_pos.z + points[i+1][j].z);

                    /* lower hemisphere */
                    glVertex3f(light_pos.x + points[i][j].x, light_pos.y + points[i][j].y, light_pos.z - points[i][j].z);
                    glVertex3f(light_pos.x + points[i][j+1].x, light_pos.y + points[i][j+1].y, light_pos.z - points[i][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j+1].x, light_pos.y + points[i+1][j+1].y, light_pos.z - points[i+1][j+1].z);
                    glVertex3f(light_pos.x + points[i+1][j].x, light_pos.y + points[i+1][j].y, light_pos.z - points[i+1][j].z);
                }
                glEnd();
            }
        }
    }

    ~SpotLight()
    {
        light_pos.x = 0;
        light_pos.y = 0;
        light_pos.z = 0;
        light_dir.x = 0;
        light_dir.y = 0;
        light_dir.z = 0;
        color.red = 0;
        color.green = 0;
        color.blue = 0;
        cutoff_angle = 0;
    }
};


class Object
{
public:
    Vector3D reference_point;
    double height, width, length;
    Color color;
    double coefficients[4];         // ambient, diffuse, specular, reflection coefficients
    int shine;                      // exponent term of specular component

    Object()
    {
        height = 0;
        width = 0;
        length = 0;
        coefficients[AMB] = 0;
        coefficients[DIFF] = 0;
        coefficients[SPEC] = 0;
        coefficients[REC_REFFLECTION] = 0;
        shine = 0;
    }

    virtual void draw() = 0;
    virtual double intersect(Ray, Color&, int) = 0;

    void setColor(double r, double g, double b)
    {
        color.setColor(r, g, b);
    }

    void setShine(int s)
    {
        shine = s;
    }

    void setCoefficients(double a, double d, double s, double r)
    {
        coefficients[AMB] = a;
        coefficients[DIFF] = d;
        coefficients[SPEC] = s;
        coefficients[REC_REFFLECTION] = r;
    }

    ~Object()
    {
        reference_point.x = 0;
        reference_point.y = 0;
        reference_point.z = 0;
        height = 0;
        width = 0;
        length = 0;
        color.red = 0;
        color.green = 0;
        color.blue = 0;
        coefficients[AMB] = 0;
        coefficients[DIFF] = 0;
        coefficients[SPEC] = 0;
        coefficients[REC_REFFLECTION] = 0;
        shine = 0;
    }
};

int recursion_level = 0;
Vector3D eye;
vector<PointLight> point_lights;
vector<SpotLight> spot_lights;
vector<Object*> objects;


class Sphere: public Object
{
    int slices;
    int stacks;

public:
    Sphere(Vector3D center, double radius)
    {
        reference_point = center;
        height = radius;
        width = radius;
        length = radius;

        slices = 72;
        stacks = 24;
    }

    void draw()
    {
        Vector3D points[stacks+1][slices+1];
        int i,j;
        double h,r;
        //generate points
        for(i=0;i<=stacks;i++)
        {
            h=length*sin(((double)i/(double)stacks)*(pi/2));
            r=length*cos(((double)i/(double)stacks)*(pi/2));
            for(j=0;j<=slices;j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }

        glColor3f(color.red, color.green, color.blue);
        //draw quads using generated points
        for(i=0;i<stacks;i++)
        {
            for(j=0;j<slices;j++)
            {
                glBegin(GL_QUADS);{
                    //upper hemisphere
                    glVertex3f(reference_point.x + points[i][j].x, reference_point.y + points[i][j].y, reference_point.z + points[i][j].z);
                    glVertex3f(reference_point.x + points[i][j+1].x, reference_point.y + points[i][j+1].y, reference_point.z + points[i][j+1].z);
                    glVertex3f(reference_point.x + points[i+1][j+1].x, reference_point.y + points[i+1][j+1].y, reference_point.z + points[i+1][j+1].z);
                    glVertex3f(reference_point.x + points[i+1][j].x, reference_point.y + points[i+1][j].y, reference_point.z + points[i+1][j].z);

                    //lower hemisphere
                    glVertex3f(reference_point.x + points[i][j].x, reference_point.y + points[i][j].y, reference_point.z - points[i][j].z);
                    glVertex3f(reference_point.x + points[i][j+1].x, reference_point.y + points[i][j+1].y, reference_point.z - points[i][j+1].z);
                    glVertex3f(reference_point.x + points[i+1][j+1].x, reference_point.y + points[i+1][j+1].y, reference_point.z - points[i+1][j+1].z);
                    glVertex3f(reference_point.x + points[i+1][j].x, reference_point.y + points[i+1][j].y, reference_point.z - points[i+1][j].z);
                }glEnd();
            }
        }
    }

    double intersect(Ray ray, Color& colorContainer, int level)
    {
        // ---------- code for finding intersecting tmin ---------- //
        double a, b, c, t1, t2, tMin;

        a = ray.Rd * ray.Rd;
        b = 2 * (ray.Rd * (ray.Ro - reference_point));
        c = (ray.Ro - reference_point) * (ray.Ro - reference_point) - length * length;

        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            tMin = INF;
        else
        {
            t1 = (-b - sqrt(discriminant)) / (2 * a);
            t2 = (-b + sqrt(discriminant)) / (2 * a);
            tMin = min(t1, t2);
        }

        // ---------------------------------------------------- //

        // -------------- Illumination with the Phong Lighting Model ------------ //

        if(level == 0)
            return tMin;

        Vector3D intersectionPoint = ray.Ro + ray.Rd * tMin;
        Color intersectionPointColor = color;
        colorContainer = intersectionPointColor * coefficients[AMB];

        // calculate normal at intersectionPoint
        Vector3D normal = intersectionPoint - reference_point;
        normal.normalize();     
        if(eye.euclideanDistance(reference_point) <= length)
            normal = normal * (-1);

        // point lights
        for (int i = 0; i < point_lights.size(); i++)
        {
            Ray ray_1(point_lights[i].light_pos, intersectionPoint - point_lights[i].light_pos);        // incident ray from light
            
            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;
            

            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (point_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (point_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // spot lights
        for (int i = 0; i < spot_lights.size(); i++)
        {
            Ray ray_1(spot_lights[i].light_pos, intersectionPoint - spot_lights[i].light_pos);        // incident ray from light

            Vector3D spotLightDir = spot_lights[i].light_dir;
            spotLightDir.normalize();
            
            double theta = acos(spotLightDir * (ray_1.Rd*(-1))) * 180/pi;
            if(theta > spot_lights[i].cutoff_angle)
                continue;
            
            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;       

            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // ---------------------------------------------------- //

        // ---------------- Recursive Reflection ---------------- //
        if(level >= recursion_level)
            return tMin;

        // construct reflected ray from intersection point
        // actually slightly forward from the point (by moving the
        // start a little bit towards the reflection direction)
        // to avoid self intersection
        Vector3D reflectionDirection = ray.Rd - normal * ((ray.Rd * normal)*2);
        reflectionDirection.normalize();                                                        // reflectionDirection is a unit vector
        Ray reflectedRay(intersectionPoint + reflectionDirection, reflectionDirection);        // start a little bit towards the reflection direction

        int nearest = -1;
        double t, tMin2 = INF;

        for(int i=0; i<objects.size(); i++)
        {
            Color dummy;
            t = objects[i]->intersect(reflectedRay, dummy, 0);
            if(t > 0 && t < tMin2)
            {
                tMin2 = t;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            // reflectionColor will be updated while in the subsequent call
            // update color using the impact of reflection
            Color reflectionColor;
            tMin2 = objects[nearest]->intersect(reflectedRay, reflectionColor, level + 1);
            colorContainer = colorContainer + reflectionColor * coefficients[REC_REFFLECTION];
        }

        // clipping color vlaues
        colorContainer.clipColor();

        return tMin;
    }
};


class Triangle: public Object
{
public:
    Vector3D a, b, c;

    Triangle(Vector3D a, Vector3D b, Vector3D c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw()
    {
        glColor3f(color.red, color.green, color.blue);
        glBegin(GL_TRIANGLES);{
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        }glEnd();
    }

    double intersect(Ray ray, Color& colorContainer, int level)
    {
        // ---------- code for finding intersecting tmin ---------- //
        double detA, detBeta, detGamma, detT, tMin, beta, gamma;

        detA = (a.x - b.x) * ( (a.y - c.y) * ray.Rd.z - (a.z - c.z) * ray.Rd.y );
        detA -= (a.x - c.x) * ( (a.y - b.y) * ray.Rd.z - (a.z - b.z) * ray.Rd.y );
        detA += ray.Rd.x * ( (a.y - b.y) * (a.z - c.z) - (a.z - b.z) * (a.y - c.y) );

        detBeta = (a.x - ray.Ro.x) * ( (a.y - c.y) * ray.Rd.z - (a.z - c.z) * ray.Rd.y );
        detBeta -= (a.x - c.x) * ( (a.y - ray.Ro.y) * ray.Rd.z - (a.z - ray.Ro.z) * ray.Rd.y );
        detBeta += ray.Rd.x * ( (a.y - ray.Ro.y) * (a.z - c.z) - (a.z - ray.Ro.z) * (a.y - c.y) );

        detGamma = (a.x - b.x) * ( (a.y - ray.Ro.y) * ray.Rd.z - (a.z - ray.Ro.z) * ray.Rd.y );
        detGamma -= (a.x - ray.Ro.x) * ( (a.y - b.y) * ray.Rd.z - (a.z - b.z) * ray.Rd.y );
        detGamma += ray.Rd.x * ( (a.y - b.y) * (a.z - ray.Ro.z) - (a.z - b.z) * (a.y - ray.Ro.y) );

        detT = (a.x - b.x) * ( (a.y - c.y) * (a.z - ray.Ro.z) - (a.z - c.z) * (a.y - ray.Ro.y) );
        detT -= (a.x - c.x) * ( (a.y - b.y) * (a.z - ray.Ro.z) - (a.z - b.z) * (a.y - ray.Ro.y) );
        detT += (a.x - ray.Ro.x) * ( (a.y - b.y) * (a.z - c.z) - (a.z - b.z) * (a.y - c.y) );

        if(detA == 0)
            tMin = INF;
        else{
            beta = detBeta / detA;
            gamma = detGamma / detA;
            tMin = detT / detA;

            if(beta <= 0 || gamma <= 0 || beta + gamma >= 1)
                tMin = INF;
        }

        // ---------------------------------------------------- //

        // -------------- Illumination with the Phong Lighting Model ------------ //

        if(level == 0)
            return tMin;

        Vector3D intersectionPoint = ray.Ro + ray.Rd * tMin;
        Color intersectionPointColor = color;
        colorContainer = intersectionPointColor * coefficients[AMB];


        // calculate normal
        Vector3D normal = (b - a) ^ (c - a);    // normal is perpendicular to both sides of the triangle
        normal.normalize();                     // normal is unit vector
        if(normal * (ray.Rd*(-1)) < 0)
            normal = normal * (-1);             // normal is pointing towards the viewer

        // point lights
        for (int i = 0; i < point_lights.size(); i++)
        {
            Ray ray_1(point_lights[i].light_pos, intersectionPoint - point_lights[i].light_pos);        // incident ray from light

            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;
            
            
            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (point_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (point_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // spot lights
        for (int i = 0; i < spot_lights.size(); i++)
        {
            Ray ray_1(spot_lights[i].light_pos, intersectionPoint - spot_lights[i].light_pos);        // incident ray from light

            Vector3D spotLightDir = spot_lights[i].light_dir;
            spotLightDir.normalize();
            
            double theta = acos(spotLightDir * (ray_1.Rd*(-1))) * 180/pi;
            if(theta > spot_lights[i].cutoff_angle)
                continue;
            
            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;          

            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // ---------------- recursive reflection ---------------- //
        if(level >= recursion_level)
            return tMin;

        // construct reflected ray from intersection point
        // actually slightly forward from the point (by moving the
        // start a little bit towards the reflection direction)
        // to avoid self intersection
        Vector3D reflectionDirection = ray.Rd - normal * ((ray.Rd * normal)*2);
        reflectionDirection.normalize();                                                        // reflectionDirection is a unit vector
        Ray reflectedRay(intersectionPoint + reflectionDirection, reflectionDirection);        // start a little bit towards the reflection direction

        int nearest = -1;
        double t, tMin2 = INF;

        for(int i=0; i<objects.size(); i++)
        {
            Color dummy;
            t = objects[i]->intersect(reflectedRay, dummy, 0);
            if(t > 0 && t < tMin2)
            {
                tMin2 = t;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            // reflectionColor will be updated while in the subsequent call
            // update color using the impact of reflection
            Color reflectionColor;
            tMin2 = objects[nearest]->intersect(reflectedRay, reflectionColor, level + 1);
            colorContainer = colorContainer + reflectionColor * coefficients[REC_REFFLECTION];
        }

        // clipping color vlaues
        colorContainer.clipColor();

        return tMin;
    }
};


class GeneralQuadricSurface : public Object
{
public:
    double A, B, C, D, E, F, G, H, I, J;
    Vector3D cube_reference_point;
    double length, width, height;

    GeneralQuadricSurface(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, Vector3D cube_reference_point, double length, double width, double height)
    {
        A = a;
        B = b;
        C = c;
        D = d;
        E = e;
        F = f;
        G = g;
        H = h;
        I = i;
        J = j;
        this->cube_reference_point = cube_reference_point;
        this->length = length;
        this->width = width;
        this->height = height;
    }

    void clip(Ray ray, double& tMin, double &tMax)
    {
        if(tMin < INF)
        {
            if(tMax < INF)
            {
                if(tMin > 0)
                {
                    Vector3D intersectionPoint = ray.Ro + ray.Rd * tMin;

                    if((length !=0 && (intersectionPoint.x<cube_reference_point.x || intersectionPoint.x>cube_reference_point.x+length)) || 
                    (width !=0 && (intersectionPoint.y<cube_reference_point.y || intersectionPoint.y>cube_reference_point.y+width)) || 
                    (height !=0 && (intersectionPoint.z<cube_reference_point.z || intersectionPoint.z>cube_reference_point.z+height)))
                        tMin = INF;
                }
                if(tMax > 0)
                {
                    Vector3D intersectionPoint = ray.Ro + ray.Rd * tMax;

                    if((length !=0 && (intersectionPoint.x<cube_reference_point.x || intersectionPoint.x>cube_reference_point.x+length)) || 
                    (width !=0 && (intersectionPoint.y<cube_reference_point.y || intersectionPoint.y>cube_reference_point.y+width)) || 
                    (height !=0 && (intersectionPoint.z<cube_reference_point.z || intersectionPoint.z>cube_reference_point.z+height)))
                        tMax = INF;
                }
                if(tMin < 0 || tMin > tMax)
                    tMin = tMax;
            }
            else{
                if(tMin > 0)
                {
                    Vector3D intersectionPoint = ray.Ro + ray.Rd * tMin;

                    if((length !=0 && (intersectionPoint.x<cube_reference_point.x || intersectionPoint.x>cube_reference_point.x+length)) || 
                    (width !=0 && (intersectionPoint.y<cube_reference_point.y || intersectionPoint.y>cube_reference_point.y+width)) || 
                    (height !=0 && (intersectionPoint.z<cube_reference_point.z || intersectionPoint.z>cube_reference_point.z+height)))
                        tMin = INF;
                }
            }
        }
    }

    void draw()
    {

    }

    double intersect(Ray ray, Color& colorContainer, int level)
    {

        // ----------------------- finding tMin ------------------- //
        double a, b, c, tMin, tMax;

        a = A * ray.Rd.x * ray.Rd.x + B * ray.Rd.y * ray.Rd.y + C * ray.Rd.z * ray.Rd.z + D * ray.Rd.x * ray.Rd.y + E * ray.Rd.x * ray.Rd.z + F * ray.Rd.y * ray.Rd.z;
        b = 2 * (A * ray.Rd.x * ray.Ro.x + B * ray.Rd.y * ray.Ro.y + C * ray.Rd.z * ray.Ro.z + D * (ray.Ro.x * ray.Rd.y + ray.Rd.x * ray.Ro.y) + E * (ray.Rd.z * ray.Ro.x + ray.Rd.x * ray.Ro.z) + F * (ray.Rd.y * ray.Ro.z + ray.Rd.z * ray.Ro.y) + G * ray.Rd.x + H * ray.Rd.y + I * ray.Rd.z);
        c = A * ray.Ro.x * ray.Ro.x + B * ray.Ro.y * ray.Ro.y + C * ray.Ro.z * ray.Ro.z + D * ray.Ro.x * ray.Ro.y + E * ray.Ro.x * ray.Ro.z + F * ray.Ro.y * ray.Ro.z + G * ray.Ro.x + H * ray.Ro.y + I * ray.Ro.z + J;
    
        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            tMin = tMax = INF;
        else if (discriminant > 0)
        {
            tMin = min((-b - sqrt(discriminant)) / (2 * a), (-b + sqrt(discriminant)) / (2 * a));
            tMax = max((-b - sqrt(discriminant)) / (2 * a), (-b + sqrt(discriminant)) / (2 * a));
        }
        else
        {
            tMin = (-b) / (2 * a);
            tMax = INF;
        }

        // clipping 
        clip(ray, tMin, tMax);

        // -------------------------------------------------------- //

        // -------------- Illumination with the Phong Lighting Model ------------ //
        
        if(level == 0)
            return tMin;

        Vector3D intersectionPoint = ray.Ro + ray.Rd * tMin;
        Color intersectionPointColor = color;
        colorContainer = intersectionPointColor * coefficients[AMB];

        // calculating normal
        double xn = 2 * A * intersectionPoint.x + D * intersectionPoint.y + E * intersectionPoint.z + G;
        double yn = 2 * B * intersectionPoint.y + D * intersectionPoint.x + F * intersectionPoint.z + H;
        double zn = 2 * C * intersectionPoint.z + E * intersectionPoint.x + F * intersectionPoint.y + I;
        Vector3D normal(xn, yn, zn);
        normal.normalize();     // normal is unit vector
        if(normal * (ray.Rd * (-1)) < 0)
            normal = normal * (-1);

        // point lights
        for (int i = 0; i < point_lights.size(); i++)
        {
            Ray ray_1(point_lights[i].light_pos, intersectionPoint - point_lights[i].light_pos);        // incident ray from light
            
            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;
            
            
            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (point_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (point_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // spot lights
        for (int i = 0; i < spot_lights.size(); i++)
        {
            Ray ray_1(spot_lights[i].light_pos, intersectionPoint - spot_lights[i].light_pos);        // incident ray from light

            Vector3D spotLightDir = spot_lights[i].light_dir;
            spotLightDir.normalize();
            
            double theta = acos(spotLightDir * (ray_1.Rd*(-1))) * 180/pi;
            if(theta > spot_lights[i].cutoff_angle)
                continue;
            
            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;           

            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // ---------------------------------------------------- //

        // ---------------- Recursive Reflection ---------------- //
        if(level >= recursion_level)
            return tMin;

        // construct reflected ray from intersection point
        // actually slightly forward from the point (by moving the
        // start a little bit towards the reflection direction)
        // to avoid self intersection
        Vector3D reflectionDirection = ray.Rd - normal * ((ray.Rd * normal)*2);
        reflectionDirection.normalize();                                                        // reflectionDirection is a unit vector
        Ray reflectedRay(intersectionPoint + reflectionDirection, reflectionDirection);        // start a little bit towards the reflection direction

        int nearest = -1;
        double t, tMin2 = INF;

        for(int i=0; i<objects.size(); i++)
        {
            Color dummy;
            t = objects[i]->intersect(reflectedRay, dummy, 0);
            if(t > 0 && t < tMin2)
            {
                tMin2 = t;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            // reflectionColor will be updated while in the subsequent call
            // update color using the impact of reflection
            Color reflectionColor;
            tMin2 = objects[nearest]->intersect(reflectedRay, reflectionColor, level + 1);
            colorContainer = colorContainer + reflectionColor * coefficients[REC_REFFLECTION];
        }

        // clipping color vlaues
        colorContainer.clipColor();

        return tMin;
    }
};



class Floor: public Object
{
    double floorWidth, tileWidth;

    public:
    Floor(double floorWidth, double tileWidth)
    {
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;

        reference_point.x = -floorWidth/2;
        reference_point.y = -floorWidth/2;
        reference_point.z = 0;

        length = tileWidth;
    }

    void draw()
    {
        double x, y;
        int rows = (int)(floorWidth/tileWidth);
        int cols = (int)(floorWidth/tileWidth);

        for(int i=0; i<rows; i++)
        {
            for(int j=0; j<cols; j++)
            {
                if((i+j)%2 == 0)
                    glColor3f(1.0, 1.0, 1.0);       // even tiles are white
                else
                    glColor3f(0.0, 0.0, 0.0);       // odd tiles are black

                x = reference_point.x + tileWidth * j;
                y = reference_point.y + tileWidth * i;

                glBegin(GL_QUADS);{
                    glVertex3f(x, y, reference_point.z);
                    glVertex3f(x + tileWidth, y, reference_point.z);
                    glVertex3f(x + tileWidth, y + tileWidth, reference_point.z);
                    glVertex3f(x, y + tileWidth, reference_point.z);
                }glEnd();
            }
        }
    }

    double intersect(Ray ray, Color& colorContainer, int level)
    {

        // ------------------------ finding tmin ------------------------ //
        Vector3D normal(0, 0, 1);

        if(eye * normal < 0)
            normal = normal * (-1);

        double tMin = INF;

        if(normal * ray.Rd != 0)
            tMin = - (normal * ray.Ro) / (normal * ray.Rd);

        Vector3D intersectionPoint = ray.Ro + ray.Rd * tMin;

        // checing if the intersection point is outside the floor
        if(tMin > 0 && tMin<INF)
        {
            if(intersectionPoint.x > -reference_point.x || intersectionPoint.x < reference_point.x || intersectionPoint.y > -reference_point.y || intersectionPoint.y < reference_point.y)
                tMin = INF;
        }


        // -------------- Illumination with the Phong Lighting Model ------------ //

        if(level == 0)
            return tMin;

        // finding the color of the intersection point
        Color intersectionPointColor;
        Vector3D refToIntersection = intersectionPoint - reference_point;
        if((int)(floor(refToIntersection.x/tileWidth) + floor(refToIntersection.y/tileWidth))%2 == 0)
            intersectionPointColor.setColor(1.0, 1.0, 1.0);
        else
            intersectionPointColor.setColor(0.0, 0.0, 0.0);

        // calculating ambient component
        colorContainer = intersectionPointColor * coefficients[AMB];

        // point lights
        for (int i = 0; i < point_lights.size(); i++)
        {
            Ray ray_1(point_lights[i].light_pos, intersectionPoint - point_lights[i].light_pos);        // incident ray from light

            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;
            
            
            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (point_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (point_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // spot lights
        for (int i = 0; i < spot_lights.size(); i++)
        {
            Ray ray_1(spot_lights[i].light_pos, intersectionPoint - spot_lights[i].light_pos);        // incident ray from light

            Vector3D spotLightDir = spot_lights[i].light_dir;
            spotLightDir.normalize();
            
            double theta = acos(spotLightDir * (ray_1.Rd*(-1))) * 180/pi;
            if(theta > spot_lights[i].cutoff_angle)
                continue;
            
            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            double t, tMin2 = INF;

            for(int i=0; i<objects.size(); i++)
            {
                Color dummy;
                t = objects[i]->intersect(ray_1, dummy, 0);
                if(t > 0 && t < tMin2)
                {
                    tMin2 = t;
                }
            }
            Vector3D shadowObstacle = ray_1.Ro + ray_1.Rd * tMin2;
            double epsilon = 0.0000001;  
            if(ray_1.Ro.euclideanDistance(shadowObstacle) < (ray_1.Ro.euclideanDistance(intersectionPoint) - epsilon))
                continue;           

            // calculate lambertValue and phongValue
            double lambertValue = max(0.0, normal * (ray_1.Rd*(-1)));

            Ray ray_r(intersectionPoint, ray_1.Rd - (normal * (ray_1.Rd * normal))*2 );               // reflected light ray
            double phongValue = pow(max(0.0, ray_r.Rd * (ray.Rd*(-1))), shine);

            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[DIFF] * lambertValue)) * intersectionPointColor;
            colorContainer = colorContainer + (spot_lights[i].color * (coefficients[SPEC] * phongValue)) * intersectionPointColor;
        }

        // ---------------- recursive reflection ---------------- //
        if(level >= recursion_level)
            return tMin;

        // construct reflected ray from intersection point
        // actually slightly forward from the point (by moving the
        // start a little bit towards the reflection direction)
        // to avoid self intersection
        Vector3D reflectionDirection = ray.Rd - normal * ((ray.Rd * normal)*2);
        reflectionDirection.normalize();                                                        // reflectionDirection is a unit vector
        Ray reflectedRay(intersectionPoint + reflectionDirection, reflectionDirection);        // start a little bit towards the reflection direction

        int nearest = -1;
        double t, tMin2 = INF;

        for(int i=0; i<objects.size(); i++)
        {
            Color dummy;
            t = objects[i]->intersect(reflectedRay, dummy, 0);
            if(t > 0 && t < tMin2)
            {
                tMin2 = t;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            // reflectionColor will be updated while in the subsequent call
            // update color using the impact of reflection
            Color reflectionColor;
            tMin2 = objects[nearest]->intersect(reflectedRay, reflectionColor, level + 1);
            colorContainer = colorContainer + reflectionColor * coefficients[REC_REFFLECTION];
        }

        // clipping color vlaues
        colorContainer.clipColor();

        return tMin;
    }
};