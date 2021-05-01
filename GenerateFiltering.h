#ifndef CECI_GENERATEFILTERING_H
#define CECI_GENERATEFILTERING_H


#include "graph.h"
#include "trees.h"

class GenerateFiltering {
public:
	static void generateCECIFilter(const Graph *data_graph, const Graph *query_graph, TreeNode *&tree,
                                       VertexID *&order);
private:
	static VertexID selectCECIStartVertex(const Graph *data_graph, const Graph *query_graph);};

#endif //CECI_GENRATEFILTERING_H
