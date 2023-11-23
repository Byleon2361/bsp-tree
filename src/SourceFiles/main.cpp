#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include "../HeaderFiles/bsp-tree.h"

vector<bsp_tree::polygon> allPolygons;
struct ply_format_exception : public std::exception
{
    const char *what() const throw()
    {
        return "Incorrect ply format";
    }
};
void load_ply(const string &filename, vector<bsp_tree::polygon> &polygons)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        throw ply_format_exception();
    }
    cout << filename << endl;
    string line;
    if (!getline(file, line))
    {
        throw ply_format_exception();
    }

    if (line != "ply")
    {
        throw ply_format_exception();
    }

    unsigned int vertices_size;
    unsigned int faces_size;

    bool header = true;
    while (header && getline(file, line))
    {
        string s;
        istringstream iss(line);

        if (!(iss >> s))
        {
            throw ply_format_exception();
        }

        if (s == "format")
        {
            if (!(iss >> s))
            {
                throw ply_format_exception();
            }

            if (s != "ascii")
            {
                throw ply_format_exception();
            }
        }
        else if (s == "element")
        {
            unsigned int n;
            if (!(iss >> s >> n))
            {
                throw ply_format_exception();
            }

            if (s == "vertex")
            {
                vertices_size = n;
            }
            else if (s == "face")
            {
                faces_size = n;
            }
        }
        else if (s == "end_header")
        {
            header = false;
        }
    }

    vector<glm::vec3> vertices;
    vertices.resize(vertices_size);

    for (unsigned int i = 0; i < vertices_size; ++i)
    {
        if (!(file >> vertices[i].x >> vertices[i].y >> vertices[i].z))
        {
            throw ply_format_exception();
        }
    }

    struct ply_polygon
    {
        unsigned int a, b, c;
    };

    vector<ply_polygon> faces;
    faces.resize(faces_size);

    for (unsigned int i = 0; i < faces_size; ++i)
    {
        unsigned int n;

        if (!(file >> n >> faces[i].a >> faces[i].b >> faces[i].c))
        {
            throw ply_format_exception();
        }

        if (n != 3)
        {
            throw ply_format_exception();
        }
    }

    polygons.resize(faces_size);

    for (unsigned int i = 0; i < faces_size; ++i)
    {
        polygons[i].p[0] = vertices[faces[i].a];
        polygons[i].p[1] = vertices[faces[i].b];
        polygons[i].p[2] = vertices[faces[i].c];
    }
}
void drawTorus()
{
    cout << "POLYGONS FROM MODEL"
         << endl;
    cout << "==================="
         << endl;
    vector<bsp_tree::polygon> polygons;

    cout << "Monkey"
         << "\n";
    load_ply("models/monkey.ply", polygons);
    // experiment(polygons);

    glBegin(GL_TRIANGLES);

    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < polygons.size(); i++)
    {
        glVertex3f(polygons[i].p[0].x, polygons[i].p[0].y, polygons[i].p[0].z);
        glVertex3f(polygons[i].p[1].x, polygons[i].p[1].y, polygons[i].p[1].z);
        glVertex3f(polygons[i].p[2].x, polygons[i].p[2].y, polygons[i].p[2].z);
    }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

    glColor3f(1.0, 0.0, 0.0);
    drawTorus();

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.1, 100.0);
}

void experiment(const vector<bsp_tree::polygon> &polygons)
{
    bsp_tree tree;
    tree.construct(polygons);

    unsigned int fragments = tree.fragments();
    unsigned int polygons_size_2 = polygons.size() * polygons.size();
    float percentage = static_cast<float>(fragments) / static_cast<float>(polygons_size_2) * 100.0f;

    cout << "Polygons: " << polygons.size() << "\n";
    cout << "Nodes: " << tree.nodes() << "\n";
    cout << "Fragments: " << fragments << "\n";
    cout << "Fragments (" << fragments << ") smaller than polygons^2 (" << polygons_size_2 << "): " << (fragments < polygons_size_2 ? "true" : "false") << " (" << setprecision(3) << fixed << percentage << "%)"
         << "\n";
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("3D Torus");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}