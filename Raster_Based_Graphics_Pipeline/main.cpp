#include <fstream>
#include <string>
#include <stack>
#include "z_buffer.h"

using namespace std;

Point pos;           /// position of the camera
Point u;             /// up direction
Point r;             /// right direction
Point l;             /// look direction


int main(int argc, char **argv)
{
    string command;
    ifstream scene;
    scene.open(input_filepath + "scene.txt");

    if(!scene.is_open())
    {
        cout<<"Could not open Scene file"<<endl;
        exit(1);
    }

    ofstream stage1(output_filepath + "stage1.txt");
    ofstream stage2(output_filepath + "stage2.txt");
    ofstream stage3(output_filepath + "stage3.txt");


    double eye_x, eye_y, eye_z;
    double look_x, look_y, look_z;
    double up_x, up_y, up_z;
    double fovY, aspectRatio, near, far;

    // taking input of first 4 lines
    scene>>eye_x>>eye_y>>eye_z;
    scene>>look_x>>look_y>>look_z;
    scene>>up_x>>up_y>>up_z;
    scene>>fovY>>aspectRatio>>near>>far;

    // converting them to points
    Point eye(eye_x, eye_y, eye_z);
    Point look(look_x, look_y, look_z);
    Point up(up_x, up_y, up_z);

    // finding l, r and u vectors
    Point l = look - eye;
    l.normalize();
    Point r = l ^ up;
    r.normalize();
    Point u = r ^ l;

    /// ---------- finding view transformation matrix ------------ ///
    TransformationMatrix T;
    T.TranslationMatrix(-eye_x, -eye_y, -eye_z);

    TransformationMatrix R;
    R.setMatrix(r, u, l*(-1));

    TransformationMatrix view_transformation_matrix = R * T;


    /// ---------- finding projection transformation matrix -------- ///
    TransformationMatrix projection_matrix;
    projection_matrix.ProjectionMatrix(fovY, aspectRatio, near, far);


    // stacks for transformation and states
    stack<TransformationMatrix> matrix_stack;
    stack<stack<TransformationMatrix>> state_stack;

    // taking an identity matrix
    TransformationMatrix identity_matrix;
    identity_matrix.IdentityMatrix();

    // pushing it onto stack
    matrix_stack.push(identity_matrix);

    while(true)
    {
        scene>>command;

        if(command == "triangle")
        {
            double point_x, point_y, point_z;
            Point points[3];

            // input three points
            for(int i=0; i<3; i++)
            {
                scene>>point_x>>point_y>>point_z;
                points[i].setPoint(point_x, point_y, point_z);
            }

            for(int i=0; i<3; i++)
            {
                /// model
                points[i] = TransformPoint(matrix_stack.top(), points[i]);
                points[i].printPoint(stage1);

                /// view
                points[i] = TransformPoint(view_transformation_matrix, points[i]);
                points[i].printPoint(stage2);

                /// projection
                points[i] = TransformPoint(projection_matrix, points[i]);
                points[i].printPoint(stage3);
            }
            stage1<<endl;
            stage2<<endl;
            stage3<<endl;
        }
        else if(command == "translate")
        {
            double tx, ty, tz;
            scene>>tx>>ty>>tz;

            TransformationMatrix translation_matrix;
            translation_matrix.TranslationMatrix(tx, ty, tz);

            matrix_stack.push(matrix_stack.top() * translation_matrix);
        }
        else if(command == "scale")
        {
            double sx, sy, sz;
            scene>>sx>>sy>>sz;

            TransformationMatrix scale_matrix;
            scale_matrix.ScaleMatrix(sx, sy, sz);

            matrix_stack.push(matrix_stack.top() * scale_matrix);
        }
        else if(command == "rotate")
        {
            double angle;
            double ax, ay, az;

            scene>>angle>>ax>>ay>>az;

            TransformationMatrix rotation_matrix;
            rotation_matrix.RotationMatrix(angle, ax, ay, az);

            matrix_stack.push(matrix_stack.top() * rotation_matrix);
        }
        else if(command == "push")
        {
            state_stack.push(matrix_stack);
        }
        else if(command == "pop")
        {
            matrix_stack = state_stack.top();
            state_stack.pop();
        }
        else if(command == "end")
        {
            break;
        }
        else
            cout<<"Action not available"<<endl;
    }

    scene.close();
    stage1.close();
    stage2.close();
    stage3.close();

    run_zBuffer_algorithm();

    return 0;
}
