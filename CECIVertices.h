#ifndef CECI_FILTERVERTICES_H
#define CECI_FILTERVERTICES_H

#include "graph.h"
#include <vector>

class CECIVertices {
public:

	static bool CECIFilter(const Graph *data_graph, const Graph *query_graph, ui **&candidates, ui *&candidates_count, ui *&order, TreeNode *&tree, std::vector<std::unordered_map<VertexID, std::vector<VertexID > > > &TE_Candidates, std::vector<std::vector<std::unordered_map<VertexID, std::vector<VertexID> > > > &NTE_Candidates);	

    	static void computeCandidateWithNLF(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                       	ui &count, ui *buffer = NULL);

    	static void computeCandidateWithLDF(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                        ui &count, ui *buffer = NULL);

    static void generateCandidates(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                      VertexID *pivot_vertices, ui pivot_vertices_count, VertexID **candidates,
                                      ui *candidates_count, ui *flag, ui *updated_flag);

    static void pruneCandidates(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                   VertexID *pivot_vertices, ui pivot_vertices_count, VertexID **candidates,
                                   ui *candidates_count, ui *flag, ui *updated_flag);

    static void printCandidatesInfo(const Graph *query_graph, ui *candidates_count);

    static void sortCandidates(ui** candidates, ui* candidates_count, ui num);

    static double computeCandidatesFalsePositiveRatio(const Graph *data_graph, const Graph *query_graph, ui **candidates,
                                                          ui *candidates_count, std::vector<ui> &optimal_candidates_count);
private:
    static void allocateBuffer(const Graph* data_graph, const Graph* query_graph, ui** &candidates, ui* &candidates_count);
    static void compactCandidates(ui** &candidates, ui* &candidates_count, ui query_vertex_num);
    static bool isCandidateSetValid(ui** &candidates, ui* &candidates_count, ui query_vertex_num);
};


#endif //CECI_FILTERVERTICES_H
