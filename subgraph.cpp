#include "subgraph.h"
#include "trees.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <memory.h>
#include <map>
#include <unordered_map>
#include <vector>
#define INVALID_VERTEX_ID 9999999

using namespace std;

V_ID InitialStartVertex(Graph *data_graph, Graph *query_graph) {
    
    // Using method of finding minimum scores = count/degree;
    // 
    
    double min_score = data_graph->getVerticesCount();
    V_ID start_vertex = 0;

    for (ui i = 0; i < query_graph->getVerticesCount(); ++i) {
        ui degree = query_graph->getVertexDegree(i);
        ui count = 0;

        // Compare the NLF with data graoh and query graph.
        
        computeNLF(data_graph, query_graph, i, count);

        double cur_score = count / (double)degree;
        if (cur_score < min_score) {
            min_score = cur_score;
            start_vertex = i;
        }
    }

    return start_vertex;
}



void bfs(Graph *graph, V_ID root_v, TreeNode *&tree, V_ID *&order) {
    // How many vertices.
    //
    
    ui count = graph->getVerticesCount();

    queue<V_ID> queue;
    vector<bool> visited(count, false);

    // Initialize the tree
    tree = new TreeNode[count];

    for (ui i = 0; i < count; ++i) {
        tree[i].initialize(count);
    }

    order = new V_ID[count];

    ui visited_count = 0;// how many nodes have been visited.
    queue.push(root_v); // push the first root node into the BFS queue.

    visited[root_v] = true;
    tree[root_v].level = 0;
    tree[root_v].id = root_v; // node id.

    while(!queue.empty()) {
        V_ID u = queue.front();
        queue.pop();
        order[visited_count++] = u; // records this as the next order.

        ui u_nbrs_count; // how many neighbors of the node 

        // get the neighbors informations
        V_ID* u_nbrs = graph->getVertexNeighbors(u, u_nbrs_count);

        for (ui i = 0; i < u_nbrs_count; ++i) {
            V_ID u_nbr = u_nbrs[i];
            // for each unvisited neighbors, if unvisited:
            // (1) push in th query
            // (2) build it in the tree
            // (3) records it parent id
            // (4) the level should add 1
            // (5) the parent node will add this nodes as child.
            if (!visited[u_nbr]) {
                queue.push(u_nbr);
                visited[u_nbr] = true;
                tree[u_nbr].id = u_nbr;
                tree[u_nbr].parent = u;
                tree[u_nbr].level = tree[u].level + 1;
                tree[u].children[tree[u].children_count++] = u_nbr;
            }
        }
    }
}


bool CECIFunction(Graph *data_graph, Graph *query_graph, ui **&candidates, ui *&candidates_count, ui *&order, TreeNode *&tree,   vector<unordered_map<V_ID, vector<V_ID > > > &TE_Candidates, vector<vector<unordered_map<V_ID, vector<V_ID> > > > &NTE_Candidates) {
    cout << "Initialize function: " << endl;
    // Initial the CECI Index.
    V_ID start_vertex = InitialStartVertex(data_graph, query_graph);  
    bfs(query_graph, start_vertex, tree, order); // Build the tree structure and order from query graph
    
    // query_count is the number of the vertexs in the query graph.
    ui query_count = query_graph->getVerticesCount();
    vector<ui> order_index(query_count);

    // Build vertex to the order Reverse index;
    for (ui i = 0; i < query_count; ++i) {
        V_ID query_vertex = order[i];
        order_index[query_vertex] = i;
    }

    // every query tree, build their neighbour and parents information. 
    cout << "Begin Building Neighbourhood Informations" << endl;

    for (ui i = 0; i < query_count; ++i) {
        V_ID u = order[i];
        tree[u].under_level_count = 0;
        tree[u].bn_count = 0;
        tree[u].fn_count = 0;

        ui u_nbrs_count;
        V_ID* u_nbrs = query_graph->getVertexNeighbors(u, u_nbrs_count);
        for (ui j = 0; j < u_nbrs_count; ++j) {
            V_ID u_nbr = u_nbrs[j];
            // If this node is not their parent nodes and has less index(which means haven't visited yet). 
            if (u_nbr != tree[u].parent && order_index[u_nbr] < order_index[u]) {
                // Recordes the parent(frontiers) node and its neighbors
                tree[u].bn[tree[u].bn_count++] = u_nbr;
                tree[u_nbr].fn[tree[u_nbr].fn_count++] = u;
            }
        }
    }

    // Initialize the Buffer for the Candidate and Candidate Count:

    ui candidates_max_count = data_graph->getGraphMaxLabelFrequency();
    
    cout << "Initialize Max Count: " << candidates_max_count << endl;
    candidates_count = new ui[query_count];
    memset(candidates_count, 0, sizeof(ui) * query_count);

    candidates = new ui*[query_count];

    for (ui i = 0; i < query_count; ++i) {
        candidates[i] = new ui[candidates_max_count];
    }

    
    ui data_count = data_graph->getVerticesCount();
    
    V_ID root = order[0];

    cout << "Root is: " << root << endl;
    computeNLF(data_graph, query_graph, root, candidates_count[root], candidates[root]);

    // No Candidates with the root
    if (candidates_count[root] == 0) {
     	cout << "Build Candidate Fail" << endl;
        return false;
    }

    // Have cancidates in the root:

    // TE_Candidates construction and filtering.
    // If the node in the data node is been visited mark as 1, else 0, can be used as cardinality in the future.;
    vector<ui> flag(data_count);

    fill(flag.begin(), flag.end(), 0);
    vector<bool> visited_query(query_count);
    fill(visited_query.begin(), visited_query.end(), false);

    visited_query[root]=true;
    TE_Candidates.resize(query_count);

    cout << "Begin BFS Function:" << endl;
    // BFS order for this tree's visit.
    //
    for (ui i = 1; i < query_count; ++i) {
        V_ID u = order[i];
        TreeNode& u_node = tree[u];
        V_ID u_p = tree[u].parent;
        
        // Records u's label and degree
        ui u_l = query_graph->getVertexLabel(u);
        ui u_d = query_graph->getVertexDegree(u);

        //get NLF of current node.

        unordered_map<L_ID, ui>* u_nlf = query_graph->getVertexNLF(u);

        candidates_count[u] = 0; // initial each candidate_count of each node to be 0.
        
        visited_query[u] = true;

        // sets of frontiers and count for parent node
        V_ID* frontiers = candidates[u_p];
        ui frontiers_count = candidates_count[u_p];
        
        for (ui j = 0; j < frontiers_count; ++j) {
            V_ID v_f = frontiers[j]; // for all its frontiers nodes
            
            // If it's not a valid one for building the CECI index, skip
            if (v_f == INVALID_VERTEX_ID)
                continue;

            ui data_nbrs_count;
            V_ID* data_nbrs = data_graph->getVertexNeighbors(v_f, data_nbrs_count);
            
            auto tmp = TE_Candidates[u].emplace(v_f, vector<V_ID>());

            //cout << endl << "Location:" << tmp << endl;
            for (ui k = 0; k < data_nbrs_count; ++k) {
                V_ID v = data_nbrs[k];

                if (data_graph->getVertexLabel(v) == u_l && data_graph->getVertexDegree(v) >= u_d) {

                    // NLF check
                    unordered_map<L_ID, ui>* v_nlf = data_graph->getVertexNLF(v);
                
                    if (v_nlf->size() >= u_nlf->size()) {
                        bool is_valid = true;

                        for (auto element : *u_nlf) {
                            auto iter = v_nlf->find(element.first);
                            if (iter == v_nlf->end() || iter->second < element.second) {
                                is_valid = false;
                                break;
                            }
                        }
                        
                        if (is_valid) {
                            tmp.first->second.push_back(v);
                            if (flag[v] == 0) {
                                flag[v] = 1;
                                candidates[u][candidates_count[u]++] = v;
                            }
                        }
                    }
                }

            }
	    if (tmp.first->second.empty()) {
		// set it as invalid
                frontiers[j] = INVALID_VERTEX_ID;
                for (ui k = 0; k < tree[u_p].children_count; ++k) {
                    V_ID u_c = tree[u_p].children[k];
                    if (visited_query[u_c]) {
                        TE_Candidates[u_c].erase(v_f);
                    }
                }
            }

        }

        if (candidates_count[u] == 0)
            return false;

        // flag reset.
        for (ui j = 0; j < candidates_count[u]; ++j) {
            V_ID v = candidates[u][j];
            flag[v] = 0; // reset the flag for the data graph.
        }

    }

    cout << endl << "Checking function: " << endl;
    for (ui i = 1; i < query_count; i++ ) {
	cout << "Current Node: " << order[i] << " ; ";
    	for (auto iter : TE_Candidates[order[i]]) {
		cout << "Vector Size: " << iter.second.size() << " ";
    		cout  << "Parent node: "  << iter.first << " [";
		for (ui j = 0; j < iter.second.size(); j++) {
			cout << iter.second[j] << ","; 
		}
		cout << " ] " << endl;	
    	}
    }


    // NTE Tree;
    //
	
    NTE_Candidates.resize(query_count);
    for (auto& item : NTE_Candidates) {
        item.resize(query_count);
    }

    for (ui i = 1; i < query_count; ++i) {
    	V_ID u = order[i];
	TreeNode & u_node = tree[u];

	ui u_l = query_graph->getVertexLabel(u);
        ui u_d = query_graph->getVertexDegree(u);
	unordered_map<L_ID, ui> *u_nlf = query_graph->getVertexNLF(u);
	
	for (ui l = 0; l < u_node.bn_count; ++l) {
		V_ID u_p = u_node.bn[l];
		V_ID *frontiers = candidates[u_p];
		ui frontiers_count = candidates_count[u_p];

		for (ui j = 0; j < frontiers_count; ++j) {
			V_ID v_f = frontiers[j];

			if (v_f == INVALID_VERTEX_ID)
				continue;

			ui data_nbrs_count;
			V_ID *data_nbrs = data_graph->getVertexNeighbors(v_f, data_nbrs_count);
			
			auto tmp = NTE_Candidates[u][u_p].emplace(v_f, vector<V_ID>());
			for (ui k = 0; k < data_nbrs_count; ++k) {
				V_ID v = data_nbrs[k];
				
				if ( data_graph->getVertexLabel(v) == u_l && data_graph->getVertexDegree(v) >= u_d ) {
					// NLF check.	
				}
			}
		}
	
	}
    }
    cout << "End of this function" << endl;
   	

    return true;
    // NTE Tree:
}
// 
// 
//
// NLF data_graph, query_graph, ID of the vertex node. total count.
//

void computeNLF(Graph *data_graph, Graph *query_graph, V_ID i, ui &count, ui *tmp) {
    L_ID label = query_graph->getVertexLabel(i);    
    ui degree = query_graph->getVertexDegree(i);
    unordered_map<L_ID, ui>* query_nlf = query_graph->getVertexNLF(i);
    
    //data vertex count;
    ui data_v_count;
    ui* data_v = data_graph->getVerticesByLabel(label, data_v_count);
    count = 0; 
    for (ui j = 0; j < data_v_count; ++j) {
        ui v = data_v[j];
        if (data_graph->getVertexDegree(v) >= degree) {
            // NFL check
            unordered_map<L_ID, ui>* data_nlf = data_graph->getVertexNLF(v);

            if (data_nlf->size() >= query_nlf->size()) {
                bool valid = true;
                
                // Label, count in the (nlf)
                for (auto item : *query_nlf) {
                    auto element = data_nlf->find(item.first);
                    if (element == data_nlf->end() || element->second < item.second) {
                        valid = false;
                        break;
                    }
                }

                if (valid) {
                    if (tmp != NULL) {
                        tmp[count] = v; // 
                    }
                    count += 1; // Recored count of the number
                }
            }
        }
    }

        
}
