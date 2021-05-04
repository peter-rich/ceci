#include "graph.h"
#include <algorithm>
#include <fstream>
void Graph::printGraph(){
	return;
}
void Graph::loadGraph(const string & file_path) {
	ifstream graphFile(file_path);

    	if (!graphFile.is_open()) {
        	std::cout << "Error opening " << file_path << " ." << std::endl;
        	exit(-1);
    	}

	char type;
    	graphFile >> type >> v_count >> e_count;
    	offsets = new ui[v_count + 1];
    	offsets[0] = 0;

	neighbors = new V_ID[e_count * 2];
	labels = new L_ID[v_count];
	l_count = 0;
	max_degree = 0;

	L_ID max_label_id = 0;
	 
	return;	
}
