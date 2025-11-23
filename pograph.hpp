// Partially-Ordered Graph
// description:

#pragma once

#include <vector>
#include <array>
#include <map>
#include <tuple>
#include <functional>
#include <algorithm>
#include <list>

#include "comparisons.hpp"
#include "dutch_sort.hpp"
#include "rank_sort.hpp"
#include "pograph_node.hpp"

// IN ALL OF THE BELOW OPTIMIZATION STEPS, MAKE SURE TO USE PROFILING TOOLS FOR CONCRETE NOTIONS OF PROGRESS
// make vectorizations more efficient by removing some unnecessary delimiters
// I fear I am using a lot of redundant information in the comparison information (both backward and forward info, ranking of boundary, residual, outward edges, etc.)
// maybe storing vertices in lists is faster
// currently, the vertex sort is a Dutch flag quicksort, but it can possibly be made more efficient by converting to radix, intro-, heap, or merge sort
// at some point, perform all "&", "*", and "const" optimizations

// NEED TO MAKE SURE TWO EDGES BETWEEN THE SAME TWO VERTICES ARE HANDLED

struct pograph {
    public:
        std::map<int, std::vector<node*>> nodes_stratified;
        std::vector<node*> nodes = {};
        std::vector<int> labels = {};
        int max_level = -1;
        int n_nodes = 0;
        long long symmetry_factor = 1;
        void read_inequalities (std::vector<std::array<int, 2>> inequalities) {
            for (auto x : inequalities) {
                int label1 = x[0];
                int label2 = x[1];
                bool cond1 = true;
                bool cond2 = true;
                int index1;
                int index2;
                for (int m = 0; m < labels.size(); m++) {
                    int y = labels[m];
                    if (y == x[0]) {
                        cond1 = false;
                        index1 = m;
                    }
                    if (y == x[1]) {
                        cond2 = false;
                        index2 = m;
                    }
                }
                node* a;
                node* b;
                if (cond1) {
                    labels.push_back(x[0]);
                    a = new node();
                    n_nodes++;
                    nodes.push_back(a);
                }
                else {
                    a = nodes[index1];
                }
                if (cond2) {
                    labels.push_back(x[1]);
                    b = new node();
                    n_nodes++;
                    nodes.push_back(b);
                }
                else {
                    b = nodes[index2];
                }
                (*a).fathers.push_back(b);
                (*b).children.push_back(a);
            }
            int m_nodes = n_nodes;
            int level = 0; 
            while (m_nodes > 0) {
                nodes_stratified[level] = {};
                max_level++;
                if (level == 0) { // can be subsumed by the below (a stylistic choice)
                    for (node* x : nodes) {
                        if ((*x).fathers.size() == 0) {
                            nodes_stratified[level].push_back(x);
                            (*x).level = 0;
                            (*x).permanent_vchildren = (*x).children;
                            m_nodes--;
                        }
                    }
                }
                else {
                    for (node* x : nodes) {
                        if (!(*x).leveled) {
                            for (node* y : (*x).fathers) {
                                if (!(*y).leveled) {
                                    goto resume_outer_loop;
                                }
                            }
                            nodes_stratified[level].push_back(x);
                            (*x).level = level;
                            for (node* z : ((*x).children)) {
                                (*x).permanent_vchildren.push_back(z);
                            }
                            for (node* z : ((*x).fathers)) {
                                (*x).permanent_vchildren.push_back(z);
                            }
                            m_nodes--;
                        }
                        resume_outer_loop:;
                    }
                }
                for (node* x : nodes_stratified[level]) {
                    (*x).leveled = true;
                }
                level++;
            }
            
            // Compute symmetry factor after graph construction is complete
            compute_symmetry_factor();
        }  
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 6>>,
            std::vector<node*>
        > 
        next_depth_a (std::vector<node*>& nodes) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::cout << "\n  [next_depth_a] Processing " << nodes.size() << " input nodes (standard)" << std::endl;
            std::vector<std::array<std::vector<int>, 6>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (node* y : (*x).vchildren_a) {
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            new_edge_info[i][0].push_back((*x).level);
                            if ((*x).ranked) {
                                new_edge_info[i][3].push_back((*x).rank);
                            }
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    if ((*x).ranked) {
                        new_edge_info.push_back({{
                            {(*x).level}, 
                            {}, {},
                            {(*x).rank}, 
                            {}, {}
                        }});
                    } 
                    else {
                        new_edge_info.push_back({{
                            {(*x).level}, 
                            {}, {}, {}, {}, {}
                        }});
                    }
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_a = {};
                for (node* y : (*new_nodes[i]).permanent_vchildren) {
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            new_edge_info[i][1].push_back((*y).level);
                            if ((*y).ranked) {
                                new_edge_info[i][4].push_back((*y).rank);
                            }
                            goto next_father;
                        }
                    }
                    new_edge_info[i][2].push_back((*y).level);
                    (*new_nodes[i]).vchildren_a.push_back(y);
                    if ((*y).ranked) {
                        new_edge_info[i][5].push_back((*y).rank);
                    }
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                sort_descending_int_vector(new_edge_info[i][0]);
                sort_descending_int_vector(new_edge_info[i][1]);
                sort_descending_int_vector(new_edge_info[i][2]);
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
            }
            sort_descending_vertex_data(new_edge_info);
            std::cout << "  [next_depth_a] Returning " << new_edge_info.size() << " edge_info entries and " << new_nodes.size() << " nodes (standard)" << std::endl;
            return std::make_tuple(new_edge_info, new_nodes);
            
        }
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 6>>,
            std::vector<node*>
        > 
        next_depth_b (std::vector<node*>& nodes) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::cout << "\n  [next_depth_b] Processing " << nodes.size() << " input nodes (standard)" << std::endl;
            std::vector<std::array<std::vector<int>, 6>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (node* y : (*x).vchildren_b) {
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            new_edge_info[i][0].push_back((*x).level);
                            if ((*x).ranked) {
                                new_edge_info[i][3].push_back((*x).rank);
                            }
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    if ((*x).ranked) {
                        new_edge_info.push_back({{
                            {(*x).level}, 
                            {}, {},
                            {(*x).rank}, 
                            {}, {}
                        }});
                    } 
                    else {
                        new_edge_info.push_back({{
                            {(*x).level}, 
                            {}, {}, {}, {}, {}
                        }});
                    }
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_b = {};
                for (node* y : (*new_nodes[i]).permanent_vchildren) {
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            new_edge_info[i][1].push_back((*y).level);
                            if ((*y).ranked) {
                                new_edge_info[i][4].push_back((*y).rank);
                            }
                            goto next_father;
                        }
                    }
                    new_edge_info[i][2].push_back((*y).level);
                    (*new_nodes[i]).vchildren_b.push_back(y);
                    if ((*y).ranked) {
                        new_edge_info[i][5].push_back((*y).rank);
                    }
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                sort_descending_int_vector(new_edge_info[i][0]);
                sort_descending_int_vector(new_edge_info[i][1]);
                sort_descending_int_vector(new_edge_info[i][2]);
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
            }
            sort_descending_vertex_data(new_edge_info);
            std::cout << "  [next_depth_b] Returning " << new_edge_info.size() << " edge_info entries and " << new_nodes.size() << " nodes (standard)" << std::endl;
            return std::make_tuple(new_edge_info, new_nodes);
            
        }
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 6>>,
            std::vector<node*>
        > 
        next_depth_resolution_a (std::vector<node*>& nodes, int& level) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::cout << "\n  [next_depth_resolution_a] Processing " << nodes.size() << " input nodes for level " << level << " (standard)" << std::endl;
            std::vector<std::array<std::vector<int>, 6>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (node* y : (*x).vchildren_a) {
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            if ((*x).level == level && (*x).ranked) {
                                new_edge_info[i][3].push_back((*x).rank);
                            }
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    if ((*x).level == level && (*x).ranked) {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {},
                            {(*x).rank}, 
                            {}, {}
                        }});
                    } 
                    else {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {}, {}, {}, {}
                        }});
                    }
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_a = {};
                for (node* y : (*new_nodes[i]).permanent_vchildren) {
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            if ((*y).level == level && (*y).ranked) {
                                new_edge_info[i][4].push_back((*y).rank);
                            }
                            goto next_father;
                        }
                    }
                    (*new_nodes[i]).vchildren_a.push_back(y);
                    if ((*y).level == level && (*y).ranked) {
                        new_edge_info[i][5].push_back((*y).rank);
                    }
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
            }
            sort_descending_vertex_data(new_edge_info);
            std::cout << "  [next_depth_resolution_a] Returning " << new_edge_info.size() << " edge_info entries and " << new_nodes.size() << " nodes (standard)" << std::endl;
            return std::make_tuple(new_edge_info, new_nodes);
        }
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 6>>,
            std::vector<node*>
        > 
        next_depth_resolution_b (std::vector<node*>& nodes, int& level) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::cout << "\n  [next_depth_resolution_b] Processing " << nodes.size() << " input nodes for level " << level << " (standard)" << std::endl;
            std::vector<std::array<std::vector<int>, 6>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (node* y : (*x).vchildren_b) {
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            if ((*x).level == level && (*x).ranked) {
                                new_edge_info[i][3].push_back((*x).rank);
                            }
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    if ((*x).level == level && (*x).ranked) {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {},
                            {(*x).rank}, 
                            {}, {}
                        }});
                    } 
                    else {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {}, {}, {}, {}
                        }});
                    }
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_b = {};
                for (node* y : (*new_nodes[i]).permanent_vchildren) {
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            if ((*y).level == level && (*y).ranked) {
                                new_edge_info[i][4].push_back((*y).rank);
                            }
                            goto next_father;
                        }
                    }
                    (*new_nodes[i]).vchildren_b.push_back(y);
                    if ((*y).level == level && (*y).ranked) {
                        new_edge_info[i][5].push_back((*y).rank);
                    }
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
            }
            sort_descending_vertex_data(new_edge_info);
            std::cout << "  [next_depth_resolution_b] Returning " << new_edge_info.size() << " edge_info entries and " << new_nodes.size() << " nodes (standard)" << std::endl;
            return std::make_tuple(new_edge_info, new_nodes);
        }
        int conclusive_vertex_comparison (node* a, node* b) {
            // Get node labels for debugging
            int label_a = -1, label_b = -1;
            for (int i = 0; i < nodes.size(); i++) {
                if (nodes[i] == a) label_a = labels[i];
                if (nodes[i] == b) label_b = labels[i];
            }
            
            std::cout << "\n=== CONCLUSIVE VERTEX COMPARISON (Standard) ===" << std::endl;
            std::cout << "Comparing vertices: A (label=" << label_a << ", level=" << (*a).level << ", rank=" << ((*a).ranked ? std::to_string((*a).rank) : "unranked") 
                      << ") vs B (label=" << label_b << ", level=" << (*b).level << ", rank=" << ((*b).ranked ? std::to_string((*b).rank) : "unranked") << ")" << std::endl;

            (*a).vchildren_a = (*a).permanent_vchildren;
            (*b).vchildren_b = (*b).permanent_vchildren;
            
            std::vector<int> a_levels = {};
            std::vector<int> a_ranks = {};
            std::cout << "\nVertex A children analysis:" << std::endl;
            for (int i = 0; i < (*a).vchildren_a.size(); i++) {
                node* x = (*a).vchildren_a[i];
                int child_label = -1;
                for (int j = 0; j < nodes.size(); j++) {
                    if (nodes[j] == x) { child_label = labels[j]; break; }
                }
                a_levels.push_back((*x).level);
                if ((*x).ranked) {
                    a_ranks.push_back((*x).rank);
                    std::cout << "  Child " << i << ": label=" << child_label << ", level=" << (*x).level 
                              << ", rank=" << (*x).rank << std::endl;
                } else {
                    std::cout << "  Child " << i << ": label=" << child_label << ", level=" << (*x).level 
                              << ", rank=unranked" << std::endl;
                }
            }
            std::vector<int> b_levels = {};
            std::vector<int> b_ranks = {};
            std::cout << "\nVertex B children analysis:" << std::endl;
            for (int i = 0; i < (*b).vchildren_b.size(); i++) {
                node* x = (*b).vchildren_b[i];
                int child_label = -1;
                for (int j = 0; j < nodes.size(); j++) {
                    if (nodes[j] == x) { child_label = labels[j]; break; }
                }
                b_levels.push_back((*x).level);
                if ((*x).ranked) {
                    b_ranks.push_back((*x).rank);
                    std::cout << "  Child " << i << ": label=" << child_label << ", level=" << (*x).level 
                              << ", rank=" << (*x).rank << std::endl;
                } else {
                    std::cout << "  Child " << i << ": label=" << child_label << ", level=" << (*x).level 
                              << ", rank=unranked" << std::endl;
                }
            }

            // std::cout << "\nBefore sorting:" << std::endl;
            // std::cout << "A - levels: [";
            // for (int i = 0; i < a_levels.size(); i++) std::cout << a_levels[i] << (i < a_levels.size()-1 ? ", " : "");
            // std::cout << "], ranks: [";
            // for (int i = 0; i < a_ranks.size(); i++) std::cout << a_ranks[i] << (i < a_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            // 
            // std::cout << "B - levels: [";
            // for (int i = 0; i < b_levels.size(); i++) std::cout << b_levels[i] << (i < b_levels.size()-1 ? ", " : "");
            // std::cout << "], ranks: [";
            // for (int i = 0; i < b_ranks.size(); i++) std::cout << b_ranks[i] << (i < b_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            
            sort_descending_int_vector(a_levels);
            sort_descending_int_vector(b_levels);
            sort_descending_int_vector(a_ranks);
            sort_descending_int_vector(b_ranks);
            
            // std::cout << "\nAfter sorting:" << std::endl;
            // std::cout << "A - levels: [";
            // for (int i = 0; i < a_levels.size(); i++) std::cout << a_levels[i] << (i < a_levels.size()-1 ? ", " : "");
            // std::cout << "], ranks: [";
            // for (int i = 0; i < a_ranks.size(); i++) std::cout << a_ranks[i] << (i < a_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            // 
            // std::cout << "B - levels: [";
            // for (int i = 0; i < b_levels.size(); i++) std::cout << b_levels[i] << (i < b_levels.size()-1 ? ", " : "");
            // std::cout << "], ranks: [";
            // for (int i = 0; i < b_ranks.size(); i++) std::cout << b_ranks[i] << (i < b_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            std::vector<std::array<std::vector<int>, 6>> a_vertex_data = {{
                {a_levels, {}, {}, a_ranks, {}, {}}
            }};
            std::vector<std::array<std::vector<int>, 6>> b_vertex_data = {{
                {b_levels, {}, {}, b_ranks, {}, {}}
            }};

            int status = conclusive_layer_comparison(
                a_vertex_data,
                b_vertex_data
            );
            if (status != 0) {
                return status;
            }

            std::vector<node*> a_nodes = {a};
            std::vector<node*> b_nodes = {b};

            std::tuple<
                std::vector<std::array<std::vector<int>, 6>>,
                std::vector<node*>
            > next_depth_info_a = next_depth_a(a_nodes);
            std::tuple
            <
                std::vector<std::array<std::vector<int>, 6>>,
                std::vector<node*>
            > next_depth_info_b = next_depth_b(b_nodes);
            int depth = 1;
            while (true) {
                std::cout << "\nDepth " << depth << " comparison:" << std::endl;
                std::cout << "A has " << std::get<1>(next_depth_info_a).size() << " nodes, B has " << std::get<1>(next_depth_info_b).size() << " nodes" << std::endl;
                
                if (std::get<1>(next_depth_info_a).size() == 0 && std::get<1>(next_depth_info_b).size() == 0) {
                    std::cout << "Both sides exhausted - vertices are equal" << std::endl;
                    std::cout << "=== COMPARISON CONCLUDED: A == B ===\n" << std::endl;
                    return 0;
                }
                
                auto& a_data = std::get<0>(next_depth_info_a);
                auto& b_data = std::get<0>(next_depth_info_b);
                std::cout << "Comparing " << a_data.size() << " A-nodes vs " << b_data.size() << " B-nodes at depth " << depth << std::endl;
                
                for (int i = 0; i < std::min(a_data.size(), b_data.size()); i++) {
                    std::cout << "  Node pair " << i << ":" << std::endl;
                    std::cout << "    A: backward_levels[0]=[";
                    for (int j = 0; j < a_data[i][0].size(); j++) std::cout << a_data[i][0][j] << (j < a_data[i][0].size()-1 ? ", " : "");
                    std::cout << "], internal_levels[1]=[";
                    for (int j = 0; j < a_data[i][1].size(); j++) std::cout << a_data[i][1][j] << (j < a_data[i][1].size()-1 ? ", " : "");
                    std::cout << "], outward_levels[2]=[";
                    for (int j = 0; j < a_data[i][2].size(); j++) std::cout << a_data[i][2][j] << (j < a_data[i][2].size()-1 ? ", " : "");
                    std::cout << "]" << std::endl;
                    std::cout << "       backward_ranks[3]=[";
                    for (int j = 0; j < a_data[i][3].size(); j++) std::cout << a_data[i][3][j] << (j < a_data[i][3].size()-1 ? ", " : "");
                    std::cout << "], internal_ranks[4]=[";
                    for (int j = 0; j < a_data[i][4].size(); j++) std::cout << a_data[i][4][j] << (j < a_data[i][4].size()-1 ? ", " : "");
                    std::cout << "], outward_ranks[5]=[";
                    for (int j = 0; j < a_data[i][5].size(); j++) std::cout << a_data[i][5][j] << (j < a_data[i][5].size()-1 ? ", " : "");
                    std::cout << "]" << std::endl;
                    
                    std::cout << "    B: backward_levels[0]=[";
                    for (int j = 0; j < b_data[i][0].size(); j++) std::cout << b_data[i][0][j] << (j < b_data[i][0].size()-1 ? ", " : "");
                    std::cout << "], internal_levels[1]=[";
                    for (int j = 0; j < b_data[i][1].size(); j++) std::cout << b_data[i][1][j] << (j < b_data[i][1].size()-1 ? ", " : "");
                    std::cout << "], outward_levels[2]=[";
                    for (int j = 0; j < b_data[i][2].size(); j++) std::cout << b_data[i][2][j] << (j < b_data[i][2].size()-1 ? ", " : "");
                    std::cout << "]" << std::endl;
                    std::cout << "       backward_ranks[3]=[";
                    for (int j = 0; j < b_data[i][3].size(); j++) std::cout << b_data[i][3][j] << (j < b_data[i][3].size()-1 ? ", " : "");
                    std::cout << "], internal_ranks[4]=[";
                    for (int j = 0; j < b_data[i][4].size(); j++) std::cout << b_data[i][4][j] << (j < b_data[i][4].size()-1 ? ", " : "");
                    std::cout << "], outward_ranks[5]=[";
                    for (int j = 0; j < b_data[i][5].size(); j++) std::cout << b_data[i][5][j] << (j < b_data[i][5].size()-1 ? ", " : "");
                    std::cout << "]" << std::endl;
                }
                
                status = conclusive_layer_comparison(
                    std::get<0>(next_depth_info_a),
                    std::get<0>(next_depth_info_b)
                );
                std::cout << "Depth " << depth << " comparison result: " << status << " (1=A>B, 2=B>A, 0=equal)" << std::endl;
                if (status != 0) {
                    std::cout << "=== COMPARISON CONCLUDED: " << (status == 1 ? "A > B" : "B > A") << " ===\n" << std::endl;
                    return status;
                }
                next_depth_info_a = next_depth_a(std::get<1>(next_depth_info_a));
                next_depth_info_b = next_depth_b(std::get<1>(next_depth_info_b));
                depth++;
            } 
        }
        std::function<int(node*, node*)> conclusive_vertex_comparison_obj = [this](node* a, node* b) { return conclusive_vertex_comparison(a, b); };
        int vertex_comparison (node* a, node* b, int& level) {
            // Get node labels for debugging
            int label_a = -1, label_b = -1;
            for (int i = 0; i < nodes.size(); i++) {
                if (nodes[i] == a) label_a = labels[i];
                if (nodes[i] == b) label_b = labels[i];
            }
            
            std::cout << "\n=== VERTEX COMPARISON (Level " << level << ") (Standard) ===" << std::endl;
            std::cout << "Comparing vertices: A (label=" << label_a << ", level=" << (*a).level << ", rank=" << ((*a).ranked ? std::to_string((*a).rank) : "unranked") 
                      << ") vs B (label=" << label_b << ", level=" << (*b).level << ", rank=" << ((*b).ranked ? std::to_string((*b).rank) : "unranked") << ") at level " << level << std::endl;

            (*a).vchildren_a = (*a).permanent_vchildren;
            (*b).vchildren_b = (*b).permanent_vchildren;
            
            std::vector<int> a_ranks = {};
            std::cout << "\nVertex A children at level " << level << ":" << std::endl;
            for (int i = 0; i < (*a).vchildren_a.size(); i++) {
                node* x = (*a).vchildren_a[i];
                int child_label = -1;
                for (int j = 0; j < nodes.size(); j++) {
                    if (nodes[j] == x) { child_label = labels[j]; break; }
                }
                if ((*x).level == level && (*x).ranked) {
                    a_ranks.push_back((*x).rank);
                    std::cout << "  Child " << i << " (at target level): label=" << child_label << ", level=" << (*x).level 
                              << ", rank=" << (*x).rank << std::endl;
                } else {
                    std::cout << "  Child " << i << " (different level or unranked): label=" << child_label << ", level=" << (*x).level 
                              << ", rank=" << ((*x).ranked ? std::to_string((*x).rank) : "unranked") << " (skipped)" << std::endl;
                }
            }
            std::vector<int> b_ranks = {};
            std::cout << "\nVertex B children at level " << level << ":" << std::endl;
            for (int i = 0; i < (*b).vchildren_b.size(); i++) {
                node* x = (*b).vchildren_b[i];
                int child_label = -1;
                for (int j = 0; j < nodes.size(); j++) {
                    if (nodes[j] == x) { child_label = labels[j]; break; }
                }
                if ((*x).level == level && (*x).ranked) {
                    b_ranks.push_back((*x).rank);
                    std::cout << "  Child " << i << " (at target level): label=" << child_label << ", level=" << (*x).level 
                              << ", rank=" << (*x).rank << std::endl;
                } else {
                    std::cout << "  Child " << i << " (different level or unranked): label=" << child_label << ", level=" << (*x).level 
                              << ", rank=" << ((*x).ranked ? std::to_string((*x).rank) : "unranked") << " (skipped)" << std::endl;
                }
            }
            
            // std::cout << "\nBefore sorting (level " << level << " only):" << std::endl;
            // std::cout << "A - ranks: [";
            // for (int i = 0; i < a_ranks.size(); i++) std::cout << a_ranks[i] << (i < a_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            // 
            // std::cout << "B - ranks: [";
            // for (int i = 0; i < b_ranks.size(); i++) std::cout << b_ranks[i] << (i < b_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            
            sort_descending_int_vector(a_ranks);
            sort_descending_int_vector(b_ranks);
            
            // std::cout << "\nAfter sorting (level " << level << " only):" << std::endl;
            // std::cout << "A - ranks: [";
            // for (int i = 0; i < a_ranks.size(); i++) std::cout << a_ranks[i] << (i < a_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            // 
            // std::cout << "B - ranks: [";
            // for (int i = 0; i < b_ranks.size(); i++) std::cout << b_ranks[i] << (i < b_ranks.size()-1 ? ", " : "");
            // std::cout << "]" << std::endl;
            std::vector<std::array<std::vector<int>, 6>> a_vertex_data = {{
                { {}, {}, {}, a_ranks, {}, {} }
            }};
            std::vector<std::array<std::vector<int>, 6>> b_vertex_data = {{
                { {}, {}, {}, b_ranks, {}, {} }
            }};

            std::cout << "\nLevel-specific layer comparison:" << std::endl;
            std::cout << "A vertex data (level " << level << "): ranks[3]=[";
            for (int i = 0; i < a_vertex_data[0][3].size(); i++) std::cout << a_vertex_data[0][3][i] << (i < a_vertex_data[0][3].size()-1 ? ", " : "");
            std::cout << "]" << std::endl;
            
            std::cout << "B vertex data (level " << level << "): ranks[3]=[";
            for (int i = 0; i < b_vertex_data[0][3].size(); i++) std::cout << b_vertex_data[0][3][i] << (i < b_vertex_data[0][3].size()-1 ? ", " : "");
            std::cout << "]" << std::endl;
            
            int status = conclusive_layer_comparison(
                a_vertex_data,
                b_vertex_data
            );
            std::cout << "Level-specific comparison result: " << status << " (1=A>B, 2=B>A, 0=equal)" << std::endl;
            if (status != 0) {
                std::cout << "=== COMPARISON CONCLUDED: " << (status == 1 ? "A > B" : "B > A") << " ===\n" << std::endl;
                return status;
            }

            std::vector<node*> a_nodes = {a};
            std::vector<node*> b_nodes = {b};

            std::tuple<
                std::vector<std::array<std::vector<int>, 6>>,
                std::vector<node*>
            > next_depth_info_a = next_depth_resolution_a(a_nodes, level);
            std::tuple
            <
                std::vector<std::array<std::vector<int>, 6>>,
                std::vector<node*>
            > next_depth_info_b = next_depth_resolution_b(b_nodes, level);
            while (true) {
                if (std::get<1>(next_depth_info_a).size() == 0 && std::get<1>(next_depth_info_b).size() == 0) {
                    return 0;
                }
                status = conclusive_layer_comparison(
                    std::get<0>(next_depth_info_a),
                    std::get<0>(next_depth_info_b)
                );
                if (status != 0) {
                    return status;
                }
                next_depth_info_a = next_depth_a(std::get<1>(next_depth_info_a));
                next_depth_info_b = next_depth_b(std::get<1>(next_depth_info_b));
            } 
        }
        std::function<int(node*, node*, int&)> vertex_comparison_obj = [this](node* a, node* b, int& c) { return vertex_comparison(a, b, c); };
        std::vector<int> vectorization () {
            int cumulative_rank = 0;
            std::vector<int> vectorized = {};
            for (int i = 0; i <= max_level; i++) {
                vectorized.push_back(nodes_stratified[i].size());
            }
            for (int i = 0; i <= max_level; i++) {
                std::vector<std::vector<node*>::iterator> indicators = dutch_sort(nodes_stratified[i].begin(), nodes_stratified[i].end(), conclusive_vertex_comparison_obj);
                // the next part of the code handles ranking within equivalence classes -- there should also be one other block like it in the codebase, maybe in rank sort
                std::vector<std::vector<node*>::iterator>::iterator it = indicators.begin();
                std::vector<std::vector<node*>::iterator>::iterator current_class_pointer = it;
                (**current_class_pointer)->rank = cumulative_rank;
                (**current_class_pointer)->ranked = true;
                cumulative_rank++;
                it++;
                while (it != indicators.end()) {
                    if (*it != nodes_stratified[i].end()) {
                        if (!(**current_class_pointer)->ranked) {
                            if (*it - *current_class_pointer > 1) {
                                rank_sort(*current_class_pointer, *it, vertex_comparison_obj, cumulative_rank, i);
                            }
                            else {
                                (**current_class_pointer)->rank = cumulative_rank;
                                (**current_class_pointer)->ranked = true;
                                cumulative_rank++;
                            }
                        }
                        else {
                            if (*it - *current_class_pointer > 2) {
                                rank_sort(*current_class_pointer + 1, *it, vertex_comparison_obj, cumulative_rank, i);
                            }
                            else if (*it - *current_class_pointer == 2) {
                                (*(*current_class_pointer + 1))->rank = cumulative_rank;
                                (*(*current_class_pointer + 1))->ranked = true;
                                cumulative_rank++;
                            }
                        }
                        current_class_pointer = it;
                    }
                    it++;
                }
                if (!(**current_class_pointer)->ranked) {
                    if (indicators.end() - current_class_pointer > 1) {
                        rank_sort(*current_class_pointer, nodes_stratified[i].end(), vertex_comparison_obj, cumulative_rank, i);
                    }
                    else {
                        (**current_class_pointer)->rank = cumulative_rank;
                        (**current_class_pointer)->ranked = true;
                        cumulative_rank++;
                    }
                }
                else {
                    if (indicators.end() - current_class_pointer > 2) {
                        rank_sort(*current_class_pointer + 1, nodes_stratified[i].end(), vertex_comparison_obj, cumulative_rank, i);
                    }
                    else if (indicators.end() - current_class_pointer == 2) {
                        (*(*current_class_pointer + 1))->rank = cumulative_rank;
                        (*(*current_class_pointer + 1))->ranked = true;
                        cumulative_rank++;
                    }
                }
                for (int j = 0; j < nodes_stratified[i].size(); j++) {
                    vectorized.push_back((*nodes_stratified[i][j]).rank);
                    std::vector<int> subvectorized = {};
                    for (node* x : (*nodes_stratified[i][j]).fathers) {
                        subvectorized.push_back((*x).rank);
                    }
                    sort_descending_int_vector(subvectorized);
                    for (int x : subvectorized) {
                        vectorized.push_back(x);
                    }
                }
            }
            return vectorized;
        }
        
        // Helper function to calculate factorial for symmetry factor computation
        long long factorial(int n) {
            if (n <= 1) return 1;
            long long result = 1;
            for (int i = 2; i <= n; i++) {
                result *= i;
            }
            return result;
        }
        
        // Compute the symmetry factor - number of permutations leaving the graph invariant
        void compute_symmetry_factor() {
            symmetry_factor = 1;
            
            // For each stratification level, find equivalent vertices
            for (int level = 0; level <= max_level; level++) {
                if (nodes_stratified.find(level) == nodes_stratified.end()) {
                    continue;
                }
                
                std::vector<node*>& level_nodes = nodes_stratified[level];
                if (level_nodes.empty()) {
                    continue;
                }
                
                // Create equivalence classes based on vertex comparison
                std::vector<std::vector<node*>> equivalence_classes;
                
                for (node* current_node : level_nodes) {
                    bool found_class = false;
                    
                    // Check if this node belongs to an existing equivalence class
                    for (auto& equiv_class : equivalence_classes) {
                        if (!equiv_class.empty()) {
                            // Compare with representative node of this class
                            if (conclusive_vertex_comparison_obj(current_node, equiv_class[0]) == 0) {
                                equiv_class.push_back(current_node);
                                found_class = true;
                                break;
                            }
                        }
                    }
                    
                    // If not found in any class, create new equivalence class
                    if (!found_class) {
                        equivalence_classes.push_back({current_node});
                    }
                }
                
                // Multiply factorials of all equivalence class sizes
                for (const auto& equiv_class : equivalence_classes) {
                    symmetry_factor *= factorial(equiv_class.size());
                }
            }
        }
};
