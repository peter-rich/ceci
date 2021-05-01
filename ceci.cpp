#include <chrono>
#include <future>
#include <thread>
#include <fstream>

#include "graph.h"
#include "GenerateFilteringPlan.h"
#include "FilterVertices.h"

// TODO:
//	(1) Build from the CSR compressed files.
//

//#include "BuildTable.h"
//#include "GenerateQueryPlan.h"
//#include "EvaluateQuery.h"

int main(int argc, char** argv){
	std::string input_query_graph_file = argv[1];
    	std::string input_data_graph_file = argv[2];

	auto start = std::chrono::high_resolution_clock::now();

	Graph* query_graph = new Graph(true);
    	query_graph->loadGraphFromFile(input_query_graph_file);

	
	Graph* data_graph = new Graph(true);
	data_graph->loadGraphFromFile(input_data_graph_file);
	
	auto end = std::chrono::high_resolution_clock::now();

    	double load_graphs_time_in_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();	
	
	std::cout << "-----" << std::endl;
    	std::cout << "Query Graph Meta Information" << std::endl;
    	query_graph->printGraphMetaData();
    	std::cout << "-----" << std::endl;
    	data_graph->printGraphMetaData();

    	std::cout << "--------------------------------------------------------------------" << std::endl;

    /**
     * Start queries.
     */	

	return 0;
}
