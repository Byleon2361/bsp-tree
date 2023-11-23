
#pragma once
#include <vector>

#define GLM_SWIZZLE
#include "glm/glm.hpp"

using namespace std;

class bsp_tree
{
public:
    struct point
    {
        float x;
        float y;
        float z;
    };

    struct polygon
    {
        glm::vec3 p[3];
    };

    ~bsp_tree();

    // Создание дерева из списка полигонов
    void construct(const vector<polygon> &polygons);

    // Возвращает количество узлов, составляющих дерево BSP
    unsigned int nodes();

    // Возвращает количество фрагментов (разбитых плоскостей)
    unsigned int fragments();

private:
    typedef glm::vec4 plane;

    struct node
    {
        node *l;
        node *r;
        vector<polygon> pols;
    };

    node *root_;

    unsigned int nodes_;
    unsigned int fragments_;

    // Выбор многоугольника, который определяет плоскость разделения
    unsigned int polygon_index(const vector<polygon> &polygons) const;

    // Построение дерева BSP(рекурсивно) из предоставленного списка полигонов
    // Правое поддерево содержит те полигоны, которые находятся перед плоскостью узла
    void construct_rec(const vector<polygon> &polygons, node *n);

    // Вычислите плоскость, заданную многоугольником
    void to_plane(const polygon &pol, plane &pl) const;

    enum dist_res
    {
        ON = 0,
        FRONT = 1,
        BACK = 2,
        HALF = 3
    };

    // Расстояние между плоскостью и многоугольником
    dist_res distance(const plane &pl, const polygon &pol) const;

    // Возвращает точку пересечения плоскости и сегмента
    void plane_segment_intersection(const bsp_tree::plane &pl, const glm::vec3 &a, const glm::vec3 &b, glm::vec3 &i) const;

    // Абстракция передней и задней сторон
    void polygon_split_aux(const bsp_tree::plane &pl, const glm::vec3 &a, const glm::vec3 &b1, const glm::vec3 &b2, vector<bsp_tree::polygon> &polygons_a, vector<bsp_tree::polygon> &polygons_b) const;

    // Разбиение полигона
    void polygon_split(const bsp_tree::plane &pl, const polygon &pol, vector<bsp_tree::polygon> &polygons_front, vector<bsp_tree::polygon> &polygons_back) const;

    // Удаление дерева
    void erase_rec(node *n);
};
