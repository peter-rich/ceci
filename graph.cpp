#include "graph.h"
#include <algorithm>
void loadGraph(const string & file_path) {
	ifstream graphFile(file_path);

    	if (!graphFile.is_open()) {
        	std::cout << "Error opening " << file_path << " ." << std::endl;
        	exit(-1);
    	}

    	char type;
    	infile >> type >> vertices_count_ >> edges_count_;
    	offsets = new ui[v_count +  1];
    	offsets[0] = 0;

    	neighbors_ = new VertexID[edges_count_ * 2];
    	labels_ = new LabelID[vertices_count_];
    	labels_count_ = 0;
    	max_degree_ = 0;

    	L_ID max_label_id = 0;
    	std::vector<ui> neighbors_offset(vertices_count_, 0);

    	while (infile >> type) {

    	}

}
