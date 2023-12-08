
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/time.h>
using namespace std;
class bsp_tree
{
public:
    struct vec3
    {
        float x;
        float y;
        float z;
    };
    struct vec4
    {
        float x;
        float y;
        float z;
        float w;
    };
    struct polygon
    {
        vec3 p[3];
    };

    typedef vec4 plane;

    struct node
    {
        node *left;
        node *right;
        vector<polygon> pols; // список всех узлов, находящизся в одной плоскости
    };
    node *root;
    enum dist_res
    {
        ON = 0,
        FRONT = 1,
        BACK = 2,
        HALF = 3
    };

    int nodes;
    int allPollygons;
    vec3 cross(vec3 u, vec3 v)
    {
        vec3 res;
        res.x = u.y * v.z - v.y * u.z;
        res.y = v.x * u.z - u.x * v.z;
        res.z = u.x * v.y - v.x * u.y;
        return res;
    }

    float dot(vec3 p1, vec3 p2)
    {
        return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
    }
    float dot(vec4 p1, vec4 p2)
    {
        return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z + p1.w * p2.w;
    }
    int polygon_index(const vector<polygon> &polygons)
    {
        return rand() % polygons.size();
    }
    void plane_segment_intersection(const plane &pl, const vec3 &a, const vec3 &b, vec3 &i)
    {
        vec3 rd = {b.x - a.x, b.y - a.y, b.z - a.z};

        float t = -(pl.w + dot(a, {pl.x, pl.y, pl.z})) / dot(rd, {pl.x, pl.y, pl.z});
        if (t >= 0 && t <= 1)
        {
            i = {a.x + t * rd.x, a.y + t * rd.y, a.z + t * rd.z};
        }
        else
        {
            i = rd;
        }
    }
    void create_new_polygons(const plane &pl, const vec3 &a, const vec3 &b1, const vec3 &b2, vector<polygon> &polygons_a, vector<polygon> &polygons_b)
    {
        vec3 i_ab1;
        plane_segment_intersection(pl, a, b1, i_ab1);

        vec3 i_ab2;
        plane_segment_intersection(pl, a, b2, i_ab2);

        polygon p_a;
        p_a.p[0] = a;
        p_a.p[1] = i_ab1;
        p_a.p[2] = i_ab2;
        polygons_a.push_back(p_a);

        polygon p_b1;
        p_b1.p[0] = b1;
        p_b1.p[1] = i_ab2;
        p_b1.p[2] = i_ab1;
        polygons_b.push_back(p_b1);

        polygon p_b2;
        p_b2.p[0] = b1;
        p_b2.p[1] = b2;
        p_b2.p[2] = i_ab2;
        polygons_b.push_back(p_b2);
    }
    void polygon_split(const plane &pl, const polygon &pol, vector<polygon> &polygons_front, vector<polygon> &polygons_back)
    {
        float d1 = dot(pl, {pol.p[0].x, pol.p[0].y, pol.p[0].z, 1});
        float d2 = dot(pl, {pol.p[1].x, pol.p[1].y, pol.p[1].z, 1});
        float d3 = dot(pl, {pol.p[2].x, pol.p[2].y, pol.p[2].z, 1});

        if (d1 <= 0 && d2 >= 0 && d3 >= 0)
        {
            create_new_polygons(pl, pol.p[0], pol.p[1], pol.p[2], polygons_front, polygons_back);
        }
        else if (d2 <= 0 && d1 >= 0 && d3 >= 0)
        {
            create_new_polygons(pl, pol.p[1], pol.p[0], pol.p[2], polygons_front, polygons_back);
        }
        else if (d3 <= 0 && d1 >= 0 && d2 >= 0)
        {
            create_new_polygons(pl, pol.p[2], pol.p[0], pol.p[1], polygons_front, polygons_back);
        }
        else if (d1 >= 0 && d2 <= 0 && d3 <= 0)
        {
            create_new_polygons(pl, pol.p[0], pol.p[1], pol.p[2], polygons_back, polygons_front);
        }
        else if (d2 >= 0 && d1 <= 0 && d3 <= 0)
        {
            create_new_polygons(pl, pol.p[1], pol.p[0], pol.p[2], polygons_back, polygons_front);
        }
        else if (d3 >= 0 && d1 <= 0 && d2 <= 0)
        {
            create_new_polygons(pl, pol.p[0], pol.p[1], pol.p[2], polygons_back, polygons_front);
        }
    }
    // вычисляем уравнение плоскости на основе многоугольника,
    // а функция расстояния определяет положение многоугольника относительно плоскости
    void to_plane(const polygon &pol, plane &pl)
    {
        vec3 u = {pol.p[1].x - pol.p[0].x, pol.p[1].y - pol.p[0].y, pol.p[1].z - pol.p[0].z};
        vec3 v = {pol.p[2].x - pol.p[0].x, pol.p[2].y - pol.p[0].y, pol.p[2].z - pol.p[0].z};

        // Вычисляем нормаль плоскости
        vec3 n = cross(u, v);

        pl.x = n.x;
        pl.y = n.y;
        pl.z = n.z;

        // Вычисляем w коэффициент плоскости путем взятия скалярного произведения вектора r и точки многоугольника
        pl.w = -dot({pl.x, pl.y, pl.z}, pol.p[0]);
    }

    dist_res distance(const plane &pl, const polygon &pol)
    {
        float d1 = dot(pl, {pol.p[0].x, pol.p[0].y, pol.p[0].z, 1});
        float d2 = dot(pl, {pol.p[1].x, pol.p[1].y, pol.p[1].z, 1});
        float d3 = dot(pl, {pol.p[2].x, pol.p[2].y, pol.p[2].z, 1});
        if (d1 == 0 && d2 == 0 && d3 == 0)
        {
            return ON;
        }
        if (d1 < 0 && d2 > 0)
        {
            return HALF;
        }
        else
        {
            if (d3 <= 0)
            {
                return BACK;
            }
            else
            {
                return FRONT;
            }
        }
    }
    void construct_bspTree(const vector<polygon> &polygons, node *n)
    {
        int pol_i = polygon_index(polygons);
        n->pols.push_back(polygons[pol_i]);

        plane pl;
        to_plane(polygons[pol_i], pl);

        vector<polygon> polygons_front;
        vector<polygon> polygons_back;

        for (unsigned int i = 0; i < polygons.size(); ++i)
        {
            if (i != pol_i)
            {
                switch (distance(pl, polygons[i]))
                {
                case ON:
                    n->pols.push_back(polygons[i]);
                    break;
                case FRONT:
                    polygons_front.push_back(polygons[i]);
                    break;

                case BACK:
                    polygons_back.push_back(polygons[i]);
                    break;

                case HALF:
                    polygon_split(pl, polygons[i], polygons_front, polygons_back);
                    break;
                }
            }
        }

        allPollygons += n->pols.size();

        if (!polygons_front.empty())
        {
            n->right = new node;
            ++nodes;
            construct_bspTree(polygons_front, n->right);
        }
        else
        {
            n->right = nullptr;
        }

        if (!polygons_back.empty())
        {
            n->left = new node;
            ++nodes;
            construct_bspTree(polygons_back, n->left);
        }
        else
        {
            n->left = nullptr;
        }
    }
    void initialization(const vector<polygon> &polygons)
    {
        if (polygons.empty())
        {
            return;
        }

        allPollygons = 0;

        root = new node;
        nodes = 1;
        construct_bspTree(polygons, root);
    }

    ~bsp_tree()
    {
        delete_bspTree(root);
    }

    void delete_bspTree(node *n)
    {
        if (n->left != nullptr)
        {
            delete_bspTree(n->left);
        }

        if (n->right != nullptr)
        {
            delete_bspTree(n->right);
        }

        delete n;
    }

    int get_nodes()
    {
        return nodes;
    }

    int get_fragments()
    {
        return allPollygons;
    }
};
double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    double res = (double)t.tv_sec + (double)t.tv_usec * 1E-6;
    return res;
}
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

    vector<bsp_tree::vec3> vertices;
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
void traverse_tree(bsp_tree::node *tree, vector<bsp_tree::polygon> &partitions)
{

    if (tree == NULL)
        return;

    if (tree->right)
    {
        traverse_tree(tree->right, partitions);
    }
    if (tree->left)
    {
        traverse_tree(tree->left, partitions);
    }
    for (int i = 0; i < tree->pols.size(); i++)
    {
        partitions.push_back(tree->pols[i]);
    }
}
void experiment(const vector<bsp_tree::polygon> &polygons)
{
    bsp_tree tree;
    double t;

    t = wtime();
    tree.initialization(polygons);
    t = wtime() - t;
    cout << "Time of construct bsp-tree: " << t << endl;

    vector<bsp_tree::polygon> partition;
    t = wtime();
    traverse_tree(tree.root, partition);
    t = wtime() - t;
    cout << "Time of traverse bsp-tree: " << t << endl;

    cout << "Polygons: " << polygons.size() << endl;
    cout << "Nodes: " << tree.get_nodes() << endl;
    cout << "Fragments: " << tree.get_fragments() << endl;
}
int main(int argc, char **argv)
{
    srand(time(NULL));
    cout << "Monkey" << endl;
    vector<bsp_tree::polygon> polygons;
    try
    {
        load_ply("models/cube.ply", polygons);
    }
    catch (const exception &ex)
    {
        cerr << "The file format must be ply. " << ex.what() << endl;
        exit(1);
    }
    experiment(polygons);

    return 0;
}