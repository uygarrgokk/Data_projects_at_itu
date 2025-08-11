#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "frac_doublelinklist.h"

typedef struct point {
    float x, y, z;
} Point;

typedef struct Triangle {
    Point *point1, *point2, *point3;
} Triangle;

typedef struct mesh {
    DoublyList triangle_array;
} Mesh;

Point* createPoint(float x, float y, float z) {
    Point* p = (Point*)malloc(sizeof(Point));
    p->x = x; p->y = y; p->z = z;
    return p;
}

Triangle* createTriangle(Point* p1, Point* p2, Point* p3) {
    Triangle* t = (Triangle*)malloc(sizeof(Triangle));
    t->point1 = p1; t->point2 = p2; t->point3 = p3;
    return t;
}

void generateFractal(Mesh* mesh, float cx, float cy, float cz, float size, int iter){
    if (iter < 0)
        return;

    float halfEdge = size * 0.5f;
    Point* corners[8];
    corners[0] = createPoint(cx - halfEdge, cy - halfEdge, cz - halfEdge);
    corners[1] = createPoint(cx + halfEdge, cy - halfEdge, cz - halfEdge);
    corners[2] = createPoint(cx + halfEdge, cy + halfEdge, cz - halfEdge);
    corners[3] = createPoint(cx - halfEdge, cy + halfEdge, cz - halfEdge);
    corners[4] = createPoint(cx - halfEdge, cy - halfEdge, cz + halfEdge);
    corners[5] = createPoint(cx + halfEdge, cy - halfEdge, cz + halfEdge);
    corners[6] = createPoint(cx + halfEdge, cy + halfEdge, cz + halfEdge);
    corners[7] = createPoint(cx - halfEdge, cy + halfEdge, cz + halfEdge);

    addBack(&mesh->triangle_array, createTriangle(corners[0], corners[3], corners[2]));
    addBack(&mesh->triangle_array, createTriangle(corners[0], corners[2], corners[1]));
    addBack(&mesh->triangle_array, createTriangle(corners[4], corners[5], corners[6]));
    addBack(&mesh->triangle_array, createTriangle(corners[4], corners[6], corners[7]));
    addBack(&mesh->triangle_array, createTriangle(corners[0], corners[4], corners[7]));
    addBack(&mesh->triangle_array, createTriangle(corners[0], corners[7], corners[3]));
    addBack(&mesh->triangle_array, createTriangle(corners[1], corners[2], corners[6]));
    addBack(&mesh->triangle_array, createTriangle(corners[1], corners[6], corners[5]));
    addBack(&mesh->triangle_array, createTriangle(corners[0], corners[1], corners[5]));
    addBack(&mesh->triangle_array, createTriangle(corners[0], corners[5], corners[4]));
    addBack(&mesh->triangle_array, createTriangle(corners[3], corners[7], corners[6]));
    addBack(&mesh->triangle_array, createTriangle(corners[3], corners[6], corners[2]));

    if (iter == 0)
        return;

    float newEdge  = halfEdge;
    float placeDist = halfEdge * 1.5f;  

    generateFractal(mesh, cx + placeDist, cy,           cz,           newEdge, iter - 1);
    generateFractal(mesh, cx - placeDist, cy,           cz,           newEdge, iter - 1);
    generateFractal(mesh, cx,           cy + placeDist, cz,           newEdge, iter - 1);
    generateFractal(mesh, cx,           cy - placeDist, cz,           newEdge, iter - 1);
    generateFractal(mesh, cx,           cy,           cz + placeDist, newEdge, iter - 1);
    generateFractal(mesh, cx,           cy,           cz - placeDist, newEdge, iter - 1);
}

void save_stl(const char* filename, Mesh* mesh) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Error opening STL file");
        return;
    }
    fprintf(f, "solid fractal_cube\n");

    Node* node = mesh->triangle_array.head;
    while (node) {
        Triangle* t = node->data;
        fprintf(f, "facet normal 0 0 0\n");
        fprintf(f, "  outer loop\n");
        fprintf(f, "    vertex %f %f %f\n", t->point1->x, t->point1->y, t->point1->z);
        fprintf(f, "    vertex %f %f %f\n", t->point2->x, t->point2->y, t->point2->z);
        fprintf(f, "    vertex %f %f %f\n", t->point3->x, t->point3->y, t->point3->z);
        fprintf(f, "  endloop\n");
        fprintf(f, "endfacet\n");
        node = node->next;
    }
    fprintf(f, "endsolid fractal_cube\n");
    fclose(f);
}

int main() {
    while (1) {
        int option;
        printf("------------------------------------\n");
        printf("1- Cube Pattern\n");
        printf("2- Quit\n");
        printf("------------------------------------\n");
        printf("Please enter an action: ");
        scanf("%d", &option);

        if (option == 1) {
            int iteration;
            printf("Enter Iteration count: ");
            scanf("%d", &iteration);

            Mesh mesh;
            initDoublyList(&mesh.triangle_array);
            generateFractal(&mesh, 0.0f, 0.0f, 0.0f, 1.0f, iteration);
            save_stl("cube_result.stl", &mesh);
            printf("STL file 'cube_result.stl' generated.\n");
        } else if (option == 2) {
            break;
        }
    }
    return 0;
}
