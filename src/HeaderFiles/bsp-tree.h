
#pragma once
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include <vector>

using namespace std;
class bsp_tree
{
public:
    struct polygon
    {
        glm::vec3 p[3];
    };

    typedef glm::vec4 plane;

    struct node
    {
        node *left;
        node *right;
        vector<polygon> pols; // список всех узлов, находящизся в одной плоскости
    };
    node *root;

    // Создание дерева из списка полигонов
    void initialization(const vector<polygon> &polygons);

    // Возвращает количество узлов, составляющих дерево BSP
    int get_nodes();

    // Возвращает количество фрагментов (разбитых плоскостей)
    int get_fragments();

    ~bsp_tree();

private:
    enum dist_res
    {
        ON = 0,
        FRONT = 1,
        BACK = 2,
        HALF = 3
    };

    int nodes;
    int allPollygons;

    // Построение bsp-tree из предоставленного списка полигонов
    void construct_bspTree(const vector<polygon> &polygons, node *n);

    // Вычислите плоскость, заданную многоугольником
    void to_plane(const polygon &pol, plane &pl) const;

    // Расстояние между плоскостью и многоугольником
    dist_res distance(const plane &pl, const polygon &pol) const;

    // Присваивает i точку пересечения плоскости и отрезка
    void plane_segment_intersection(const bsp_tree::plane &pl, const glm::vec3 &a, const glm::vec3 &b, glm::vec3 &i) const;

    void create_new_polygons(const plane &pl, const glm::vec3 &a, const glm::vec3 &b1, const glm::vec3 &b2, vector<polygon> &polygons_a, vector<polygon> &polygons_b) const;

    // Разбиение полигона
    void polygon_split(const plane &pl, const polygon &pol, vector<polygon> &polygons_front, vector<polygon> &polygons_back) const;

    // Выбор рандомного многоугольника, который определяет плоскость разделения
    int polygon_index(const vector<polygon> &polygons) const;

    // Удаление дерева
    void delete_bspTree(node *n);
};