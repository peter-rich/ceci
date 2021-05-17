#include "graph.h"
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;
void Graph::printGraph(){
    cout << "|V|: " << v_count << ", |E|: " << e_count << ", |L|: " << l_count << endl;
    cout << "Max Degree: " << max_degree << ", Max Label Frequency: " << max_label_frequency << endl;
    printf("Neighbourhood: ");
    //for (int i = 0; i < v_count; i ++) {
    //    printf("%d, ", offsets[i]);
    //
    //}
    printf("\n");
    for (int i = 0; i < v_count; i ++) {
        printf("V_ID: %d, offsets: %d:", i, offsets[i]);
        for (int j = offsets[i]; j < offsets[i+1]; j++) {
            printf("%d, ", neighbors[j]);
	    cout << " (" << getVertexLabel(neighbors[j]) << ") ";
        }
        printf("\n");
    }     
    
    printf("\n");
}

void Graph::BuildReverseIndex() { 
    reverse_index = new ui[v_count];
    reverse_index_offsets= new ui[l_count + 1];
    reverse_index_offsets[0] = 0;

    ui total = 0;
    for (ui i = 0; i < l_count; ++i) {
        reverse_index_offsets[i + 1] = total;
        total += labels_frequency[i];
    }

    for (ui i = 0; i < v_count; ++i) {
        L_ID label = labels[i];
        reverse_index[reverse_index_offsets[label + 1]++] = i;
    }
}

void Graph::BuildNLCF(){ // neighbors count 
	
    // Map from [label_id] -> count of this label count value.
    nlf = new unordered_map<L_ID, ui>[v_count];

    for (ui i = 0; i < v_count; ++i) {
        ui count;
    	V_ID * neighbors_tmp = getVertexNeighbors(i, count);

		// count is the number of the offsets[id + 1] - offsets[id]; which is the number of neighbours.

        for (ui j = 0; j < count; ++j) {
        		V_ID u = neighbors_tmp[j];
            	L_ID label = getVertexLabel(u);
                // If not found, this should be 0;
            	if (nlf[i].find(label) == nlf[i].end()) {
                	nlf[i][label] = 0;
            	}

            	nlf[i][label] += 1; // Add one after each count.
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
			labels_frequency[label] += 1;
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
	cout << "Test All: " << endl;

	for (auto item : labels_frequency) {
		cout << item.second << " ";
		if (item.second > max_label_frequency) {
			max_label_frequency = item.second;
		}
	} 

	BuildReverseIndex();
	BuildNLCF();
}
