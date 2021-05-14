#ifndef SUBGRAPH_H
#define SUBGRAPH_H

#include "graph.h"
#include <vector>
#include "trees.h"
using namespace std;


bool CECIFunction(Graph *data_graph, Graph *query_graph, ui **&candidates, ui *&candidates_count, ui *&order, TreeNode *&tree,   vector<unordered_map<V_ID, vector<V_ID > > > &TE_Candidates, vector<vector<unordered_map<V_ID, vector<V_ID> > > > &NTE_Candidates);
void Initial(Graph* data_graph, Graph* query_graph, ui&candidates, ui* & candidates_count);
V_ID InitialStartVertex(Graph *data_graph, Graph *query_graph);
void computeNLF(Graph *data_graph, Graph *query_graph, V_ID i, ui &count, ui *tmp = NULL);   

#endif //SUBGRAPH_H
