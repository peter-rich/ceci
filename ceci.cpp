#include <chrono>
#include <future>
#include <thread>
#include <fstream>

#include "graph.h"
#include "GenerateFiltering.h"
#include "CECIVertices.h"

// TODO:
//	(1) Build from the CSR compressed files.
//	(2) Build Reverse Refinement
//	(3) Print_out the Tree.
//
using namespace std;

int main(int argc, char** argv){
	string input_query_graph_file = argv[1];
    	string input_data_graph_file = argv[2];

	auto start = chrono::high_resolution_clock::now();

	Graph* query_graph = new Graph(true);
    	query_graph->loadGraphFromFile(input_query_graph_file);

	
	Graph* data_graph = new Graph(true);
	data_graph->loadGraphFromFile(input_data_graph_file);
	
	auto end = chrono::high_resolution_clock::now();

    	double load_graphs_time_in_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();	
	
	cout << "-----" << endl;
    	cout << "Query Graph Meta Information" << endl;
    	query_graph->printGraphMetaData();
    	cout << "-----" << endl;
    	data_graph->printGraphMetaData();

    	cout << "--------------------------------------------------------------------" << endl;

    /**
     * Start queries.
     */	
	cout << "Start queries..." << endl;
    	cout << "-----" << endl;
    	start = chrono::high_resolution_clock::now();

	ui** candidates = NULL;
    	ui* candidates_count = NULL;
    	TreeNode* ceci_tree = NULL;	
	ui* ceci_order = NULL;

    	vector<unordered_map<VertexID, vector<VertexID >>> TE_Candidates;
    	vector<vector<unordered_map<VertexID, vector<VertexID>>>> NTE_Candidates;
	CECIVertices::CECIFilter(data_graph, query_graph, candidates, candidates_count, ceci_order, ceci_tree, TE_Candidates, NTE_Candidates);

	end = chrono::high_resolution_clock::now();
    	double filter_vertices_time_in_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	cout << "-----\n";
	cout << "Filter time: " <<filter_vertices_time_in_ns/1000000000 << " s" << endl;
	cout << "-----" << endl;
    	cout << "Build indices..." << endl;

	//CECIVertices::printCandidatesInfo(query_graph, candidates_count);	
	
	return 0;
}
