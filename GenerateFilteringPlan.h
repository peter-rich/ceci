#ifndef CECI_GENERATEFILTERINGPLAN_H
#define CECI_GENERATEFILTERINGPLAN_H


#include "graph.h"
#include "trees.h"

class GenerateFilteringPlan {
public:
	static void generateCECIFilterPlan(const Graph *data_graph, const Graph *query_graph, TreeNode *&tree,
                                       VertexID *&order);
private:
	static VertexID selectCECIStartVertex(const Graph *data_graph, const Graph *query_graph);};

#endif //CECI_GENRATEFILTERINGPLAN_H
