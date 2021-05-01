#include "CECIVertices.h"
#include "GenerateFiltering.h"
#include <memory.h>
#include "graphoperations.h"
#include <vector>
#include <algorithm>
#define INVALID_VERTEX_ID 99999
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

                if (data_graph->getVertexLabel(v) == u_label && data_graph->getVertexDegree(v) >= u_degree) {


                    iter_pair.first->second.push_back(v);
                    if (flag[v] == 0) {
                        flag[v] = 1;
                        candidates[u][candidates_count[u]++] = v;
                    }
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
                        iter_pair.first->second.push_back(v);
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
            // NFL check
            if (buffer != NULL) {
                buffer[count] = data_vertex;
            }
            count += 1;
        }
    }

}

// Degree Filter
void CECIVertices::computeCandidateWithLDF(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                             ui &count, ui *buffer) {
    LabelID label = query_graph->getVertexLabel(query_vertex);
    ui degree = query_graph->getVertexDegree(query_vertex);
    count = 0;
    ui data_vertex_num;
    const ui* data_vertices = data_graph->getVerticesByLabel(label, data_vertex_num);

    if (buffer == NULL) {
        for (ui i = 0; i < data_vertex_num; ++i) {
            VertexID v = data_vertices[i];
            if (data_graph->getVertexDegree(v) >= degree) {
                count += 1;
            }
        }
    }
    else {
        for (ui i = 0; i < data_vertex_num; ++i) {
            VertexID v = data_vertices[i];
            if (data_graph->getVertexDegree(v) >= degree) {
                buffer[count++] = v;
            }
        }
    }
}

void CECIVertices::generateCandidates(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                       VertexID *pivot_vertices, ui pivot_vertices_count, VertexID **candidates,
                                       ui *candidates_count, ui *flag, ui *updated_flag) {
    LabelID query_vertex_label = query_graph->getVertexLabel(query_vertex);
    ui query_vertex_degree = query_graph->getVertexDegree(query_vertex);
    ui count = 0;
    ui updated_flag_count = 0;
    for (ui i = 0; i < pivot_vertices_count; ++i) {
        VertexID pivot_vertex = pivot_vertices[i];

        for (ui j = 0; j < candidates_count[pivot_vertex]; ++j) {
            VertexID v = candidates[pivot_vertex][j];

            ui v_nbrs_count;
            const VertexID* v_nbrs = data_graph->getVertexNeighbors(v, v_nbrs_count);

            for (ui k = 0; k < v_nbrs_count; ++k) {
                VertexID v_nbr = v_nbrs[k];
                LabelID v_nbr_label = data_graph->getVertexLabel(v_nbr);
                ui v_nbr_degree = data_graph->getVertexDegree(v_nbr);

                if (flag[v_nbr] == count && v_nbr_label == query_vertex_label && v_nbr_degree >= query_vertex_degree) {
                    flag[v_nbr] += 1;

                    if (count == 0) {
                        updated_flag[updated_flag_count++] = v_nbr;
                    }
                }
            }
        }

        count += 1;
    }

    for (ui i = 0; i < updated_flag_count; ++i) {
        VertexID v = updated_flag[i];
        if (flag[v] == count) {
            // NLF filter.
            candidates[query_vertex][candidates_count[query_vertex]++] = v;
        }
    }

    for (ui i = 0; i < updated_flag_count; ++i) {
        ui v = updated_flag[i];
        flag[v] = 0;
    }
}

void CECIVertices::pruneCandidates(const Graph *data_graph, const Graph *query_graph, VertexID query_vertex,
                                    VertexID *pivot_vertices, ui pivot_vertices_count, VertexID **candidates,
                                    ui *candidates_count, ui *flag, ui *updated_flag) {
    LabelID query_vertex_label = query_graph->getVertexLabel(query_vertex);
    ui query_vertex_degree = query_graph->getVertexDegree(query_vertex);

    ui count = 0;
    ui updated_flag_count = 0;
    for (ui i = 0; i < pivot_vertices_count; ++i) {
        VertexID pivot_vertex = pivot_vertices[i];

        for (ui j = 0; j < candidates_count[pivot_vertex]; ++j) {
            VertexID v = candidates[pivot_vertex][j];

            ui v_nbrs_count;
            const VertexID* v_nbrs = data_graph->getVertexNeighbors(v, v_nbrs_count);

            for (ui k = 0; k < v_nbrs_count; ++k) {
                VertexID v_nbr = v_nbrs[k];
                LabelID v_nbr_label = data_graph->getVertexLabel(v_nbr);
                ui v_nbr_degree = data_graph->getVertexDegree(v_nbr);

                if (flag[v_nbr] == count && v_nbr_label == query_vertex_label && v_nbr_degree >= query_vertex_degree) {
                    flag[v_nbr] += 1;

                    if (count == 0) {
                        updated_flag[updated_flag_count++] = v_nbr;
                    }
                }
            }
        }

        count += 1;
    }

    for (ui i = 0; i < candidates_count[query_vertex]; ++i) {
        ui v = candidates[query_vertex][i];

        if (flag[v] != count) {
            candidates[query_vertex][i] = INVALID_VERTEX_ID;
        }
    }

    for (ui i = 0; i < updated_flag_count; ++i) {
        ui v = updated_flag[i];
        flag[v] = 0;
    }
}

void CECIVertices::printCandidatesInfo(const Graph *query_graph, ui *candidates_count, std::vector<ui> &optimal_candidates_count) {
    std::vector<std::pair<VertexID, ui>> core_vertices;
    std::vector<std::pair<VertexID, ui>> tree_vertices;
    std::vector<std::pair<VertexID, ui>> leaf_vertices;

    ui query_vertices_num = query_graph->getVerticesCount();
    double sum = 0;
    double optimal_sum = 0;
    for (ui i = 0; i < query_vertices_num; ++i) {
        VertexID cur_vertex = i;
        ui count = candidates_count[cur_vertex];
        sum += count;
        optimal_sum += optimal_candidates_count[cur_vertex];

        if (query_graph->getCoreValue(cur_vertex) > 1) {
            core_vertices.emplace_back(std::make_pair(cur_vertex, count));
        }
        else {
            if (query_graph->getVertexDegree(cur_vertex) > 1) {
                tree_vertices.emplace_back(std::make_pair(cur_vertex, count));
            }
            else {
                leaf_vertices.emplace_back(std::make_pair(cur_vertex, count));
            }
        }
    }

    printf("#Candidate Information: CoreVertex(%zu), TreeVertex(%zu), LeafVertex(%zu)\n", core_vertices.size(), tree_vertices.size(), leaf_vertices.size());

    for (auto candidate_info : core_vertices) {
        printf("CoreVertex %u: %u, %u \n", candidate_info.first, candidate_info.second, optimal_candidates_count[candidate_info.first]);
    }

    for (auto candidate_info : tree_vertices) {
        printf("TreeVertex %u: %u, %u\n", candidate_info.first, candidate_info.second, optimal_candidates_count[candidate_info.first]);
    }

    for (auto candidate_info : leaf_vertices) {
        printf("LeafVertex %u: %u, %u\n", candidate_info.first, candidate_info.second, optimal_candidates_count[candidate_info.first]);
    }

    printf("Total #Candidates: %.1lf, %.1lf\n", sum, optimal_sum);
}

void CECIVertices::sortCandidates(ui **candidates, ui *candidates_count, ui num) {
    for (ui i = 0; i < num; ++i) {
        std::sort(candidates[i], candidates[i] + candidates_count[i]);
    }
}

double CECIVertices::computeCandidatesFalsePositiveRatio(const Graph *data_graph, const Graph *query_graph, ui **candidates,
                                                    ui *candidates_count, std::vector<ui> &optimal_candidates_count) {
    ui query_vertices_count = query_graph->getVerticesCount();
    ui data_vertices_count = data_graph->getVerticesCount();

    std::vector<std::vector<ui> > candidates_copy(query_vertices_count);
    for (ui i = 0; i < query_vertices_count; ++i) {
        candidates_copy[i].resize(candidates_count[i]);
        std::copy(candidates[i], candidates[i] + candidates_count[i], candidates_copy[i].begin());
    }

    std::vector<int> flag(data_vertices_count, 0);
    std::vector<ui> updated_flag;
    std::vector<double> per_query_vertex_false_positive_ratio(query_vertices_count);
    optimal_candidates_count.resize(query_vertices_count);

    bool is_steady = false;
    while (!is_steady) {
        is_steady = true;
        for (ui i = 0; i < query_vertices_count; ++i) {
            ui u = i;

            ui u_nbr_cnt;
            const ui *u_nbrs = query_graph->getVertexNeighbors(u, u_nbr_cnt);

            ui valid_flag = 0;
            for (ui j = 0; j < u_nbr_cnt; ++j) {
                ui u_nbr = u_nbrs[j];

                for (ui k = 0; k < candidates_count[u_nbr]; ++k) {
                    ui v = candidates_copy[u_nbr][k];

                    if (v == INVALID_VERTEX_ID)
                        continue;

                    ui v_nbr_cnt;
                    const ui *v_nbrs = data_graph->getVertexNeighbors(v, v_nbr_cnt);

                    for (ui l = 0; l < v_nbr_cnt; ++l) {
                        ui v_nbr = v_nbrs[l];

                        if (flag[v_nbr] == valid_flag) {
                            flag[v_nbr] += 1;

                            if (valid_flag == 0) {
                                updated_flag.push_back(v_nbr);
                            }
                        }
                    }
                }
                valid_flag += 1;
            }

            for (ui j = 0; j < candidates_count[u]; ++j) {
                ui v = candidates_copy[u][j];

                if (v == INVALID_VERTEX_ID)
                    continue;

                if (flag[v] != valid_flag) {
                    candidates_copy[u][j] = INVALID_VERTEX_ID;
                    is_steady = false;
                }
            }

            for (auto v : updated_flag) {
                flag[v] = 0;
            }
            updated_flag.clear();
        }
    }

    double sum = 0;
    for (ui i = 0; i < query_vertices_count; ++i) {
        ui u = i;
        ui negative_count = 0;
        for (ui j = 0; j < candidates_count[u]; ++j) {
            ui v = candidates_copy[u][j];

            if (v == INVALID_VERTEX_ID)
                negative_count += 1;
        }

        per_query_vertex_false_positive_ratio[u] =
                (negative_count) / (double) candidates_count[u];
        sum += per_query_vertex_false_positive_ratio[u];
        optimal_candidates_count[u] = candidates_count[u] - negative_count;
    }

    return sum / query_vertices_count;
}
