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
	std::vector<ui> neighbors_offsets(v_count, 0);
	while(graphFile >> type) {
		if (type == 'v') { // Read Vertex
			V_ID id;
			L_ID label;
			ui degree;
			graphFile >> id >> label >> degree;
			
			labels[id] = label;
			offsets[id+1] = offsets[id] + degree;
			if (degree > max_degree) {
				max_degree = degree;
			}
			
			if (labels_frequency.find(label) == labels_frequency.end()) {
	labels_frequency[label] = 0;
	if ( label > max_label_id) {
		max_label_id = label;	
	}
				
			}
		}
		else if (type == "e") { // Read edge.
			V_ID src;
			V_ID dst;
			graphFile >> src >> dst;

			ui offset_tmp = offset[src] + neighbors_offset[src]
		}
						
		
	}	 
	return;	
}
