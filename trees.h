#ifndef CECI_TYPES_H
#define CECI_TYPES_H 

#include <cstdint>
#include <stdlib.h>

typedef unsigned int ui; // u is the query node.

typedef uint32_t V_ID;
typedef ui L_ID;



class TreeNode {
public:
    V_ID id;
    V_ID parent;
    ui level;
    ui under_level_count;
    ui children_count;
    ui bn_count;
    ui fn_count;
    V_ID* under_level;
    V_ID* children;
    V_ID* bn;
    V_ID* fn;
    size_t estimated_embeddings_num;
public:
    TreeNode() {
        id = 0;
        under_level = NULL;
        bn = NULL;
        fn = NULL;
        children = NULL;
        parent = 0;
        level = 0;
        under_level_count = 0;
        children_count = 0;
        bn_count = 0;
        fn_count = 0;
        estimated_embeddings_num = 0;
    }

    ~TreeNode() {
        delete[] under_level;
        delete[] bn;
        delete[] fn;
        delete[] children;
    }

    void initialize(const ui size) {
        under_level = new V_ID[size];
        bn = new V_ID[size];
        fn = new V_ID[size];
        children = new V_ID[size];
    }
};

class Edges {
public:
    ui* offset;
    ui* edge;
    ui v_count;
    ui e_count;
    ui max_degree;
public:
    Edges() {
        offset = NULL;
        edge = NULL;
        v_count = 0;
        e_count = 0;
        max_degree = 0;
    }

    ~Edges() {
        delete[] offset;
        delete[] edge;
    }
};

#endif //CECI_TYPES_H
