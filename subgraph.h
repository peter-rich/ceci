#ifndef SUBGRAPH_H
#define SUBGRAPH_H

#include "graph.h"
#include <vector>
using namespace std;
class Subgraph{
public:
    bool CECI(Graph *data_graph, Graph *query_graph, ui **&candidates, ui *&candidates_count,
                           ui *&order, TreeNode *&tree,   vector<unordered_map<V_ID, vector<V_ID >>> &TE_Candidates,
                           vector<vector<unordered_map<V_ID, vector<V_ID>>>> &NTE_Candidates);
};

#endif //SUBGRAPH_H
