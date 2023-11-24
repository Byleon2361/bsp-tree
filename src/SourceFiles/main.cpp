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
void load_ply(const string &filename, vector<bsp_tree::polygon> &polygons)
{
    ifstream file(filename);
    if (!file.is_open())
        throw logic_error("Failed to open file");

    string line;
    if (!getline(file, line))
        throw logic_error("Failed to read file");

    if (line != "ply")
        throw logic_error("The file does not contain ply");

    unsigned int vertices_size;
    unsigned int faces_size;

    bool header = true;
    while (header && getline(file, line))
    {
        string s;
        stringstream iss(line);

        if (!(iss >> s))
            throw logic_error("Failed to read file");

        if (s == "format")
        {
            if (!(iss >> s))
                throw logic_error("The file does not contain format");

            if (s != "ascii")
                throw logic_error("The file does not contain ascii");
        }
        else if (s == "element")
        {
            unsigned int n;
            if (!(iss >> s >> n))
                throw logic_error("Failed to read file");

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
            throw logic_error("Failed to read coordinates in file");
    }

    struct ply_polygon
    {
        unsigned int a, b, c;
    };

    vector<ply_polygon> faces;
    faces.resize(faces_size);
    polygons.resize(faces_size);
    for (unsigned int i = 0; i < faces_size; ++i)
    {
        unsigned int n;

        if (file >> n >> faces[i].a >> faces[i].b >> faces[i].c)
        {
            polygons[i].p[0] = vertices[faces[i].a];
            polygons[i].p[1] = vertices[faces[i].b];
            polygons[i].p[2] = vertices[faces[i].c];
        }
        else
            throw logic_error("Failed to read faces in file");

        if (n != 3)
            throw logic_error("All polygons must be triangles");
    }
}
void experiment(const vector<bsp_tree::polygon> &polygons)
{
    bsp_tree tree;
    tree.construct(polygons);

    unsigned int fragments = tree.get_fragments();
    unsigned int polygons_size_2 = polygons.size() * polygons.size();
    float percentage = static_cast<float>(fragments) / static_cast<float>(polygons_size_2) * 100.0f;

    cout << "Polygons: " << polygons.size() << endl;
    cout << "Nodes: " << tree.get_nodes() << endl;
    cout << "Fragments: " << fragments << endl;
    cout << "Fragments (" << fragments << ") smaller than polygons^2 (" << polygons_size_2 << "): " << (fragments < polygons_size_2 ? "true" : "false") << " (" << setprecision(3) << fixed << percentage << "%)" << endl;
}
void drawTorus()
{
    cout << "Monkey" << endl;
    vector<bsp_tree::polygon> polygons;
    try
    {
        load_ply("models/monkey.ply", polygons);
    }
    catch (const exception &ex)
    {
        cerr << "The file format must be ply. " << ex.what() << endl;
        exit(1);
    }
    experiment(polygons);

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
    gluLookAt(1.0, 1.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

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
int main(int argc, char **argv)
{
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("3D Models");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}