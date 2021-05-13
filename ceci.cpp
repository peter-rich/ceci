#include <fstream>
#include "graph.h"

// TODO:
//	(1) Build from the CSR compressed files.
//	(2) Build Reverse Refinement
//	(3) Print_out the Tree.
//
using namespace std;

int main(int argc, char** argv){
	string input_query_graph_file = argv[1];
    	string input_data_graph_file = argv[2];


	Graph* query_graph = new Graph();
    	query_graph->loadGraph(input_query_graph_file);
	
	Graph* data_graph = new Graph();
	data_graph->loadGraph(input_data_graph_file);
	
	cout << "-----" << endl;
    	cout << "Query Graph Meta Information" << endl;
    	query_graph->printGraph();
    	cout << "-----" << endl;
    	data_graph->printGraph();

    	cout << "--------------------------------------------------------------------" << endl;

    /**
     * Start queries.
     */	
	cout << "Start queries..." << endl;
    	cout << "-----" << endl;

	ui** candidates = NULL;
    ui* candidates_count = NULL;
    TreeNode* ceci_tree = NULL;	
	ui* ceci_order = NULL;
	
	//vector<unordered_map<V_ID, vector<V_ID>>> TE_Candidates; // <v_p, {set of v_c}>
	//vector<>
	
	return 0;
}
