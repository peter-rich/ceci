#ifndef CECI_TYPES_H
#define CECI_TYPES_H 

#include <cstdint>
#include <stdlib.h>

typedef unsigned int ui; // ui is the query node.

typedef uint32_t V_ID;
typedef ui L_ID;



class TreeNode {
public:
    V_ID id_;
    V_ID parent_;
    ui level_;
    ui under_level_count_;
    ui children_count_;
    ui bn_count_;
    ui fn_count_;
    V_ID* under_level_;
    V_ID* children_;
    V_ID* bn_;
    V_ID* fn_;
    size_t estimated_embeddings_num_;
public:
    TreeNode() {
        id_ = 0;
        under_level_ = NULL;
        bn_ = NULL;
        fn_ = NULL;
        children_ = NULL;
        parent_ = 0;
        level_ = 0;
        under_level_count_ = 0;
        children_count_ = 0;
        bn_count_ = 0;
        fn_count_ = 0;
        estimated_embeddings_num_ = 0;
    }

    ~TreeNode() {
        delete[] under_level_;
        delete[] bn_;
        delete[] fn_;
        delete[] children_;
    }

    void initialize(const ui size) {
        under_level_ = new V_ID[size];
        bn_ = new V_ID[size];
        fn_ = new V_ID[size];
        children_ = new V_ID[size];
    }
};

class Edges {
public:
    ui* offset_;
    ui* edge_;
    ui vertex_count_;
    ui edge_count_;
    ui max_degree_;
public:
    Edges() {
        offset_ = NULL;
        edge_ = NULL;
        vertex_count_ = 0;
        edge_count_ = 0;
        max_degree_ = 0;
    }

    ~Edges() {
        delete[] offset_;
        delete[] edge_;
    }
};

#endif //CECI_TYPES_H
