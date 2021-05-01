#ifndef CECI_GRAPHOPERATIONS_H
#define CECI_GRAPHOPERATIONS_H

#include "graph.h"
class GraphOperations {
public:
    static void getKCore(const Graph *graph, int *core_table);
    static void match_bfs(int* col_ptrs, int* col_ids, int* match, int* row_match, int* visited,
                          int* queue, int* previous, int n, int m);
    static void bfsTraversal(const Graph *graph, VertexID root_vertex, TreeNode *&tree, VertexID *&bfs_order);
    static void old_cheap(int* col_ptrs, int* col_ids, int* match, int* row_match, int n, int m);
};


#endif
