#include "CECIVertices.h" 
#include "GenerateFiltering.h"
#include <memory.h>
#include "graphoperations.h"
#include <vector>
#include <algorithm>
#define INVALID_VERTEX_ID 99999
using namespace std;

bool
CECIVertices::CECIFilter(const Graph *data_graph, const Graph *query_graph, ui **&candidates, ui *&candidates_count,
                           ui *&order, TreeNode *&tree,  std::vector<std::unordered_map<VertexID, std::vector<VertexID >>> &TE_Candidates,
                           std::vector<std::vector<std::unordered_map<VertexID, std::vector<VertexID>>>> &NTE_Candidates) {
    GenerateFiltering::generateCECIFilter(data_graph, query_graph, tree, order);

    allocateBuffer(data_graph, query_graph, candidates, candidates_count);

    ui query_vertices_count = query_graph->getVerticesCount();
    ui data_vertices_count = data_graph->getVerticesCount();
    // Find the pivots.
    VertexID root = order[0];
    computeCandidateWithNLF(data_graph, query_graph, root, candidates_count[root], candidates[root]);
	
    cout << candidates_count[root] << endl;    
    if (candidates_count[root] == 0)
        return false;

    // TE_Candidates construction and filtering.
    std::vector<ui> updated_flag(data_vertices_count);
    std::vector<ui> flag(data_vertices_count);
    std::fill(flag.begin(), flag.end(), 0);
    std::vector<bool> visited_query_vertex(query_vertices_count);
    std::fill(visited_query_vertex.begin(), visited_query_vertex.end(), false);

    visited_query_vertex[root] = true;

    TE_Candidates.resize(query_vertices_count);

    for (ui i = 1; i < query_vertices_count; ++i) {
        VertexID u = order[i];
        TreeNode& u_node = tree[u];
        VertexID u_p = tree[u].parent_;

        ui u_label = query_graph->getVertexLabel(u);
        ui u_degree = query_graph->getVertexDegree(u);

	// Label filter initiation.
	const std::unordered_map<LabelID, ui>* u_nlf = query_graph->getVertexNLF(u);

        candidates_count[u] = 0;

        visited_query_vertex[u] = true;
        VertexID* frontiers = candidates[u_p];
        ui frontiers_count = candidates_count[u_p];

        for (ui j = 0; j < frontiers_count; ++j) {
            VertexID v_f = frontiers[j];
		
	    if (v_f == INVALID_VERTEX_ID)
                continue;

            ui nbrs_cnt;
            const VertexID* nbrs = data_graph->getVertexNeighbors(v_f, nbrs_cnt);

            auto iter_pair = TE_Candidates[u].emplace(v_f, std::vector<VertexID>());
            for (ui k = 0; k < nbrs_cnt; ++k) {
                VertexID v = nbrs[k];
		// Label Degree Filter
                if (data_graph->getVertexLabel(v) == u_label && data_graph->getVertexDegree(v) >= u_degree) {


		    // NLF check
		    const std::unordered_map<LabelID, ui>* v_nlf = data_graph->getVertexNLF(v);

		    if (v_nlf->size() >= u_nlf->size()){
		    	bool is_valid = true;

			for (auto element : *u_nlf) {
				auto iter = v_nlf->find(element.first);
				if (iter == v_nlf->end() || iter->second < element.second) {
					is_valid = false;
					break;
				}
			}

			if (is_valid) {
				iter_pair.first->second.push_back(v);
				if (flag[v] == 0) {
					flag[v] = 1;
					candidates[u][candidates_count[u]++] = v;
				}
			}
		    }



                    /* No Label Count Filter.
		     * iter_pair.first->second.push_back(v);
                    if (flag[v] == 0) {
                        flag[v] = 1;
                        candidates[u][candidates_count[u]++] = v;
                    }*/
                }
            }

            if (iter_pair.first->second.empty()) {
                frontiers[j] = INVALID_VERTEX_ID;
                for (ui k = 0; k < tree[u_p].children_count_; ++k) {
                    VertexID u_c = tree[u_p].children_[k];
                    if (visited_query_vertex[u_c]) {
                        TE_Candidates[u_c].erase(v_f);
                    }
                }
            }
        }

        if (candidates_count[u] == 0)
            return false;

        for (ui j = 0; j < candidates_count[u]; ++j) {
            VertexID v = candidates[u][j];
            flag[v] = 0;
        }
    }

    // NTE_Candidates construction and filtering.
    NTE_Candidates.resize(query_vertices_count);
    for (auto& value : NTE_Candidates) {
        value.resize(query_vertices_count);
    }

    for (ui i = 1; i < query_vertices_count; ++i) {
        VertexID u = order[i];
        TreeNode &u_node = tree[u];

        ui u_label = query_graph->getVertexLabel(u);
        ui u_degree = query_graph->getVertexDegree(u);

	// Label Count Filter Initiate
	const std::unordered_map<LabelID, ui> *u_nlf = query_graph->getVertexNLF(u);

	// Program.

        for (ui l = 0; l < u_node.bn_count_; ++l) {
            VertexID u_p = u_node.bn_[l];
            VertexID *frontiers = candidates[u_p];
            ui frontiers_count = candidates_count[u_p];

            for (ui j = 0; j < frontiers_count; ++j) {
                VertexID v_f = frontiers[j];

                if (v_f == INVALID_VERTEX_ID)
                    continue;

                ui nbrs_cnt;
                const VertexID *nbrs = data_graph->getVertexNeighbors(v_f, nbrs_cnt);

                auto iter_pair = NTE_Candidates[u][u_p].emplace(v_f, std::vector<VertexID>());
                for (ui k = 0; k < nbrs_cnt; ++k) {
                    VertexID v = nbrs[k];

                    if (data_graph->getVertexLabel(v) == u_label && data_graph->getVertexDegree(v) >= u_degree) {

                        // NLF check
			// Initiate label count of the v
			const std::unordered_map<LabelID, ui> *v_nlf = data_graph->getVertexNLF(v);

			if (v_nlf->size() >= u_nlf->size()){
				bool is_valid = true;

				for (auto element : *u_nlf) {
					auto iter = v_nlf->find(element.first);
					if (iter == v_nlf->end() || iter->second < element.second) {
						is_valid = false;
						break;
					}
				}

				if (is_valid) {
					iter_pair.first->second.push_back(v);
				}
			}

                        // This situation will not use the label count of the v.
			// iter_pair.first->second.push_back(v);
                    }
                }

                if (iter_pair.first->second.empty()) {
                    frontiers[j] = INVALID_VERTEX_ID;
                    for (ui k = 0; k < tree[u_p].children_count_; ++k) {
                        VertexID u_c = tree[u_p].children_[k];
                        TE_Candidates[u_c].erase(v_f);
                    }
                }
            }
        }
    }
    
    // Reverse BFS refine build vector for every query.
    std::vector<std::vector<ui>> cardinality(query_vertices_count);
    for (ui i = 0; i < query_vertices_count; ++i) {
    	cardinality[i].resize(candidates_count[i], 1);
    }

    std::vector<ui> local_cardinality(data_vertices_count);
    std::fill(local_cardinality.begin(), local_cardinality.end(), 0);
	
    for (int i = query_vertices_count - 1; i >= 0; --i) {
        VertexID u = order[i];
        TreeNode& u_node = tree[u];

        ui flag_num = 0;
        ui updated_flag_count = 0;

	//compute the intersection of the TE_Candidates and NTE_Candidates.
	for (ui j = 0; j < candidates_count[u]; ++j) {
		VertexID v = candidates[u][j];

		if (v == INVALID_VERTEX_ID)
			continue;

		if (flag[v] == flag_num) {
			flag[v] += 1;
			updated_flag[updated_flag_count++] = v;
		}
		for (ui j = 0; j < u_node.bn_count_; ++j) {
			VertexID u_bn = u_node.bn_[j];
			flag_num += 1;
			for (auto iter = NTE_Candidates[u][u_bn].begin(); iter != NTE_Candidates[u][u_bn].end(); ++iter) {
                		for (auto v : iter->second) {
                    			if (flag[v] == flag_num) {
                        			flag[v] += 1;
                    			}
                		}
            		}
		}
	}

	flag_num += 1;

	// Get the cardinality of the candidates of u.
	for (ui j = 0; j < candidates_count[u]; ++j) {
		VertexID v = candidates[u][j];
		if (v != INVALID_VERTEX_ID && flag[v] == flag_num) {
			local_cardinality[v] = cardinality[u][j];
		}
		else {
			cardinality[u][j] = 0;
		}
	}

	VertexID u_p = u_node.parent_;
	VertexID* frontiers = candidates[u_p];
	ui frontiers_count = candidates_count[u_p];

	// Loop over TE_Candidates.
	//
	for (ui j = 0; j < frontiers_count; ++j) {	
		VertexID v_f = frontiers[j];	
		
		if (v_f == INVALID_VERTEX_ID) {
			cardinality[u_p][j] = 0;
			continue;
		}

		ui temp_score = 0;
		for (auto iter = TE_Candidates[u][v_f].begin(); iter != TE_Candidates[u][v_f].end();) {
			VertexID v = *iter;
			temp_score += local_cardinality[v];
			if (local_cardinality[v] == 0) {
				iter = TE_Candidates[u][v_f].erase(iter);
                    		for (ui k = 0; k < u_node.children_count_; ++k) {
                        		VertexID u_c = u_node.children_[k];
                        		TE_Candidates[u_c].erase(v);
                    		}

                    		for (ui k = 0; k < u_node.fn_count_; ++k) {
                        		VertexID u_c = u_node.fn_[k];
                       			NTE_Candidates[u_c][u].erase(v);
                    		}	
			}
			else {
				++iter;
			}
		}

		cardinality[u_p][j] *= temp_score;
	}

	// Clear updated flag.
	//
	for (ui j = 0; j < updated_flag_count; ++j) {
            flag[updated_flag[j]] = 0;
            local_cardinality[updated_flag[j]] = 0;
        }
    }
    
    compactCandidates(candidates, candidates_count, query_vertices_count);
    sortCandidates(candidates, candidates_count, query_vertices_count);

    // Clear NTE_Candidate and TE_Candidate

    // Check if candidate is none then return false
    for (ui i = 0; i < query_vertices_count; ++ i){
    	if (candidates_count[i] == 0) {
		return false;
	}
    }

    //
    for (ui i = 1; i < query_vertices_count; ++i) {
    	VertexID u = order[i];
	TreeNode& u_node = tree[u];

	//Clear TE_Candidates.
	{
		VertexID u_p = u_node.parent_;
		auto iter = TE_Candidates[u].begin();
		while(iter != TE_Candidates[u].end()) {
			VertexID v_f = iter->first;
			if (!std::binary_search(candidates[u_p], candidates[u_p] + candidates_count[u_p], v_f)) {
				iter = TE_Candidates[u].erase(iter);
			}	
			else {
				std::sort(iter->second.begin(), iter->second.end());
				iter++;
			}
		}
	}

	// Clear NTE_Candidates.
	{
		for (ui j = 0; j < u_node.bn_count_; ++j) {
			VertexID u_p = u_node.bn_[j];
			auto iter = NTE_Candidates[u][u_p].end();
			while (iter != NTE_Candidates[u][u_p].end()) {
				VertexID v_f = iter->first;
				if (!std::binary_search(candidates[u_p], candidates[u_p] + candidates_count[u_p], v_f)) {
                        		iter = NTE_Candidates[u][u_p].erase(iter);
                    		}
                    		else {
                        		std::sort(iter->second.begin(), iter->second.end());
                        		iter++;
                    		}
			}
		}
	}
    }



    return true;
}

void CECIVertices::allocateBuffer(const Graph *data_graph, const Graph *query_graph, ui **&candidates,
                                    ui *&candidates_count) {
    ui query_vertex_num = query_graph->getVerticesCount();
    ui candidates_max_num = data_graph->getGraphMaxLabelFrequency();

    candidates_count = new ui[query_vertex_num];
    memset(candidates_count, 0, sizeof(ui) * query_vertex_num);

    candidates = new ui*[query_vertex_num];

    for (ui i = 0; i < query_vertex_num; ++i) {
        candidates[i] = new ui[candidates_max_num];
    }
}



void CECIVertices::compactCandidates(ui **&candidates, ui *&candidates_count, ui query_vertex_num) {
    for (ui i = 0; i < query_vertex_num; ++i) {
        VertexID query_vertex = i;
        ui next_position = 0;
        for (ui j = 0; j < candidates_count[query_vertex]; ++j) {
            VertexID data_vertex = candidates[query_vertex][j];

            if (data_vertex != INVALID_VERTEX_ID) {
                candidates[query_vertex][next_position++] = data_vertex;
            }
        }

        candidates_count[query_vertex] = next_position;
    }
}

bool CECIVertices::isCandidateSetValid(ui **&candidates, ui *&candidates_count, ui query_vertex_num) {
    for (ui i = 0; i < query_vertex_num; ++i) {
        if (candidates_count[i] == 0)
            return false;
    }
    return true;
}

void CECIVertices::computeCandidateWithNLF(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                               ui &count, ui *buffer) {
    LabelID label = query_graph->getVertexLabel(query_vertex);
    ui degree = query_graph->getVertexDegree(query_vertex);
    ui data_vertex_num;
    const ui* data_vertices = data_graph->getVerticesByLabel(label, data_vertex_num);
    count = 0;
    for (ui j = 0; j < data_vertex_num; ++j) {
        ui data_vertex = data_vertices[j];
        if (data_graph->getVertexDegree(data_vertex) >= degree) {
            // NFL check/
            if (buffer != NULL) {
                buffer[count] = data_vertex;
            }
            count += 1;
        }
    }

}


void CECIVertices::sortCandidates(ui **candidates, ui *candidates_count, ui num) {
    for (ui i = 0; i < num; ++i) {
        std::sort(candidates[i], candidates[i] + candidates_count[i]);
    }
}

