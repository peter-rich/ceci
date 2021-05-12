#ifndef CECI_GRAPH_H
#define CECI_GRAPH_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include "trees.h"
using namespace std;

class Graph {
private:
	ui v_count; // vertices count
	ui e_count; // edges count
	ui l_count; // Label count
	ui max_degree;
	ui max_label_frequency;

	ui* offsets;
	
	V_ID * neighbors;
	L_ID * labels;

	unordered_map<L_ID, ui> labels_frequency;
	ui* labels_offsets;
    	unordered_map<L_ID, ui>* nlf;
	
private:
	void BuildNLCF();
	void BuildLabelOffset();

public:
	Graph(){
		v_count = 0; // vertices count
        	e_count = 0; // edges count
        	l_count = 0; // Label count
        	max_degree = 0;
        	max_label_frequency = 0;

        	offsets = NULL;

        	neighbors = NULL;
        	labels = NULL;

        	labels_frequency.clear();
        	labels_offsets = NULL;
        	nlf = NULL;

	}
	~Graph() {
		delete[] offsets;
		delete[] neighbors;
		delete[] labels;
		delete[] labels_offsets;
		delete[] nlf;
	}
public:
	void loadGraph(const string & file_path);
	//void loadCompressedGraph(const );
	void printGraph();
	ui getLabelsCount() {
		return l_count;
	} 

	ui getVerticesCount() {
		return v_count;
	}
	
	ui getEdgesCount() {
		return e_count;
	}

	ui getMaxDegree() {
		return max_degree;
	}

	L_ID getVertexLabel(const V_ID id) {
        	return labels[id];
    	}

	ui *getVertexNeighbors(const V_ID id, ui& count){
        	count = offsets[id + 1] - offsets[id];
        	return neighbors + offsets[id];
    	}
};

#endif
