#include "graph.h"
#include <algorithm>
#include <fstream>

using namespace std;
void Graph::printGraph(){
	return;
}
// Build Index from the to label to the vertex.
void Graph::BuildReverseIndex(){
	reverse_index = new ui[v_count];
	reverse_index_offsets = new ui[l_count + 1];
	reverse_index_offsets[0] = 0;

	ui total = 0;
	for (ui i = 0; i < l_count; ++i) {
		reverse_index_offsets[i+1] = total;
		total += labels_frequency[i];
	}
	
	for (ui i = 0; i < v_count; ++ i) {
		L_ID label = labels[i];
		reverse_index[reverse_index_offsets[label + 1] ++] = i;
	}	
}

void Graph::BuildNLCF(){
	nlf = new unordered_map<L_ID, ui>[v_count];
    	for (ui i = 0; i < v_count; ++i) {
        	ui count;
        	V_ID * neighbors_tmp = getVertexNeighbors(i, count);

		// count is the number of the offsets_[id + 1] - offsets_[id]; which is the number of neighbours.

        	for (ui j = 0; j < count; ++j) {
            		V_ID u = neighbors_tmp[j];
            		L_ID label = getVertexLabel(u);
            		if (nlf[i].find(label) == nlf[i].end()) {
                		nlf[i][label] = 0;
            		}

            		nlf[i][label] += 1;
        	}
    	}
	
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
	vector<ui> neighbors_offsets(v_count, 0);
	while(graphFile >> type) {
		if (type == 'v') { // Read Vertex, build index of id->label, records its degree.
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
		else if (type == 'e') { // Read edge.
			V_ID src;
			V_ID dst;
			graphFile >> src >> dst;

			ui offset_tmp = offsets[src] + neighbors_offsets[src];
			neighbors[offset_tmp] = dst;
		
			offset_tmp = offsets[dst] + neighbors_offsets[dst];
			neighbors[offset_tmp] = src;
			
			neighbors_offsets[src] += 1;
			neighbors_offsets[dst] += 1;	
			
		}
						
		
	}	
	
	graphFile.close();

	// Initialize label count;
	if (labels_frequency.size() > max_label_id + 1) {
		l_count = labels_frequency.size(); 
	}
	else {
		l_count = max_label_id + 1;
	} 
	
	// Recheck the max_label_id;
	for (auto item : labels_frequency) {
		if (item.second > max_label_frequency) {
			max_label_frequency = item.second;
		}
	} 

	
	BuildReverseIndex();
	BuildNLCF();
}
