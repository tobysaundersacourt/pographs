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
#include <numeric> // for std::iota
#include <iostream>
#include <cstddef> // for std::size_t

#include "comparisons.hpp"
#include "dutch_sort.hpp"
#include "rank_sort_bicolored.hpp"
#include "pograph_node_bicolored.hpp"
#include "respect_sort.hpp"

// IN ALL OF THE BELOW OPTIMIZATION STEPS, MAKE SURE TO USE PROFILING TOOLS FOR CONCRETE NOTIONS OF PROGRESS
// make vectorizations more efficient by removing some unnecessary delimiters
// I fear I am using a lot of redundant information in the comparison information (both backward and forward info, ranking of boundary, residual, outward edges, etc.)
// maybe storing vertices in lists is faster
// currently, the vertex sort is a Dutch flag quicksort, but it can possibly be made more efficient by converting to radix, intro-, heap, or merge sort
// at some point, perform all "&", "*", and "const" optimizations

// NEED TO MAKE SURE TWO EDGES BETWEEN THE SAME TWO VERTICES ARE HANDLED

template <typename T>
std::vector<std::size_t> sort_indices(const std::vector<T> &v) {

  // initialize original index locations
  std::vector<std::size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values
  std::stable_sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});

  return idx;
}

struct pograph {
    public:
        std::map<int, std::vector<node*>> nodes_stratified;
        std::vector<node*> nodes = {};
        std::vector<int> labels = {};
        int max_level = -1;
        int n_nodes = 0;
        void read_inequalities (std::vector<std::array<int, 3>> inequalities) {
            // Sort inequalities lexicographically to ensure canonical input order
            std::sort(inequalities.begin(), inequalities.end());
            
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
                (*a).fathers_edge_colors.push_back(x[2]);
                (*b).children_edge_colors.push_back(x[2]);
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
                            // Sort children by label for canonical order
                            std::vector<std::pair<int, int>> label_color_pairs; // (label, color)
                            for (int i = 0; i < (*x).children.size(); i++) {
                                node* child = (*x).children[i];
                                // Find label of child node
                                int child_label = -1;
                                for (int j = 0; j < nodes.size(); j++) {
                                    if (nodes[j] == child) {
                                        child_label = labels[j];
                                        break;
                                    }
                                }
                                label_color_pairs.push_back({child_label, (*x).children_edge_colors[i]});
                            }
                            // Sort by label for canonical order
                            std::sort(label_color_pairs.begin(), label_color_pairs.end());
                            
                            (*x).permanent_vchildren.clear();
                            (*x).permanent_vchildren_edge_colors.clear();
                            for (const auto& pair : label_color_pairs) {
                                // Find node with this label
                                for (int j = 0; j < nodes.size(); j++) {
                                    if (labels[j] == pair.first) {
                                        (*x).permanent_vchildren.push_back(nodes[j]);
                                        (*x).permanent_vchildren_edge_colors.push_back(pair.second);
                                        break;
                                    }
                                }
                            }
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
                            // Sort all children and fathers by label for canonical order
                            std::vector<std::pair<int, int>> all_label_color_pairs; // (label, color)
                            
                            // Add children
                            for (int i = 0; i < (*x).children.size(); i++) {
                                node* child = (*x).children[i];
                                int child_label = -1;
                                for (int j = 0; j < nodes.size(); j++) {
                                    if (nodes[j] == child) {
                                        child_label = labels[j];
                                        break;
                                    }
                                }
                                all_label_color_pairs.push_back({child_label, (*x).children_edge_colors[i]});
                            }
                            
                            // Add fathers
                            for (int i = 0; i < (*x).fathers.size(); i++) {
                                node* father = (*x).fathers[i];
                                int father_label = -1;
                                for (int j = 0; j < nodes.size(); j++) {
                                    if (nodes[j] == father) {
                                        father_label = labels[j];
                                        break;
                                    }
                                }
                                all_label_color_pairs.push_back({father_label, (*x).fathers_edge_colors[i]});
                            }
                            
                            // Sort by label for canonical order
                            std::sort(all_label_color_pairs.begin(), all_label_color_pairs.end());
                            
                            (*x).permanent_vchildren.clear();
                            (*x).permanent_vchildren_edge_colors.clear();
                            for (const auto& pair : all_label_color_pairs) {
                                // Find node with this label
                                for (int j = 0; j < nodes.size(); j++) {
                                    if (labels[j] == pair.first) {
                                        (*x).permanent_vchildren.push_back(nodes[j]);
                                        (*x).permanent_vchildren_edge_colors.push_back(pair.second);
                                        break;
                                    }
                                }
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
        }  
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 9>>,
            std::vector<node*>
        > 
        next_depth_a (std::vector<node*>& nodes) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::vector<std::array<std::vector<int>, 9>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (int edge_idx = 0; edge_idx < (*x).vchildren_a.size(); edge_idx++) {
                    node* y = (*x).vchildren_a[edge_idx];
                    int edge_color = (*x).vchildren_a_edge_colors[edge_idx];
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            new_edge_info[i][0].push_back((*x).level);
                            new_edge_info[i][6].push_back(edge_color);
                            new_edge_info[i][3].push_back((*x).ranked ? (*x).rank : -1);
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    new_edge_info.push_back({{
                        {(*x).level}, 
                        {}, {},
                        {(*x).ranked ? (*x).rank : -1}, 
                        {}, {},
                        {edge_color},
                        {}, {}
                    }});
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_a = {};
                (*new_nodes[i]).vchildren_a_edge_colors = {};
                for (int perm_idx = 0; perm_idx < (*new_nodes[i]).permanent_vchildren.size(); perm_idx++) {
                    node* y = (*new_nodes[i]).permanent_vchildren[perm_idx];
                    int edge_color = (*new_nodes[i]).permanent_vchildren_edge_colors[perm_idx];
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            new_edge_info[i][1].push_back((*y).level);
                            new_edge_info[i][7].push_back(edge_color);
                            new_edge_info[i][4].push_back((*y).ranked ? (*y).rank : -1);
                            goto next_father;
                        }
                    }
                    new_edge_info[i][2].push_back((*y).level);
                    new_edge_info[i][8].push_back(edge_color);
                    (*new_nodes[i]).vchildren_a.push_back(y);
                    (*new_nodes[i]).vchildren_a_edge_colors.push_back(edge_color);
                    new_edge_info[i][5].push_back((*y).ranked ? (*y).rank : -1);
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                // Use respect_sort to maintain level-color correspondence
                respect_sort(new_edge_info[i][0], new_edge_info[i][6]);
                respect_sort(new_edge_info[i][1], new_edge_info[i][7]);
                respect_sort(new_edge_info[i][2], new_edge_info[i][8]);
                // Sort ranks independently (they correspond to the already sorted levels)
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
            }
            sort_descending_vertex_data(new_edge_info);
            return std::make_tuple(new_edge_info, new_nodes);
            
        }
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 9>>,
            std::vector<node*>
        > 
        next_depth_b (std::vector<node*>& nodes) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::vector<std::array<std::vector<int>, 9>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (int edge_idx = 0; edge_idx < (*x).vchildren_b.size(); edge_idx++) {
                    node* y = (*x).vchildren_b[edge_idx];
                    int edge_color = (*x).vchildren_b_edge_colors[edge_idx];
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            new_edge_info[i][0].push_back((*x).level);
                            new_edge_info[i][6].push_back(edge_color);
                            new_edge_info[i][3].push_back((*x).ranked ? (*x).rank : -1);
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    if ((*x).ranked) {
                        new_edge_info.push_back({{
                            {(*x).level}, 
                            {}, {},
                            {(*x).rank}, 
                            {}, {},
                            {edge_color},
                            {}, {}
                        }});
                    } 
                    else {
                        new_edge_info.push_back({{
                            {(*x).level}, 
                            {}, {}, {}, {}, {},
                            {edge_color},
                            {}, {}
                        }});
                    }
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_b = {};
                (*new_nodes[i]).vchildren_b_edge_colors = {};
                for (int perm_idx = 0; perm_idx < (*new_nodes[i]).permanent_vchildren.size(); perm_idx++) {
                    node* y = (*new_nodes[i]).permanent_vchildren[perm_idx];
                    int edge_color = (*new_nodes[i]).permanent_vchildren_edge_colors[perm_idx];
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            new_edge_info[i][1].push_back((*y).level);
                            new_edge_info[i][7].push_back(edge_color);
                            new_edge_info[i][4].push_back((*y).ranked ? (*y).rank : -1);
                            goto next_father;
                        }
                    }
                    new_edge_info[i][2].push_back((*y).level);
                    new_edge_info[i][8].push_back(edge_color);
                    (*new_nodes[i]).vchildren_b.push_back(y);
                    (*new_nodes[i]).vchildren_b_edge_colors.push_back(edge_color);
                    new_edge_info[i][5].push_back((*y).ranked ? (*y).rank : -1);
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                // Use respect_sort to maintain level-color correspondence
                respect_sort(new_edge_info[i][0], new_edge_info[i][6]);
                respect_sort(new_edge_info[i][1], new_edge_info[i][7]);
                respect_sort(new_edge_info[i][2], new_edge_info[i][8]);
                // Sort ranks independently (they correspond to the already sorted levels)
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
            }
            sort_descending_vertex_data(new_edge_info);
            return std::make_tuple(new_edge_info, new_nodes);
            
        }
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 9>>,
            std::vector<node*>
        > 
        next_depth_resolution_a (std::vector<node*>& nodes, int& level) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::vector<std::array<std::vector<int>, 9>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (int edge_idx = 0; edge_idx < (*x).vchildren_a.size(); edge_idx++) {
                    node* y = (*x).vchildren_a[edge_idx];
                    int edge_color = (*x).vchildren_a_edge_colors[edge_idx];
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            if ((*x).level == level) {
                                new_edge_info[i][3].push_back((*x).ranked ? (*x).rank : -1);
                                new_edge_info[i][6].push_back(edge_color);
                            }
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    if ((*x).level == level) {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {},
                            {(*x).ranked ? (*x).rank : -1}, 
                            {}, {},
                            {edge_color},
                            {}, {}
                        }});
                    } 
                    else {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {}, {}, {}, {},
                            {}, {}, {}
                        }});
                    }
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_a = {};
                (*new_nodes[i]).vchildren_a_edge_colors = {};
                for (int perm_idx = 0; perm_idx < (*new_nodes[i]).permanent_vchildren.size(); perm_idx++) {
                    node* y = (*new_nodes[i]).permanent_vchildren[perm_idx];
                    int edge_color = (*new_nodes[i]).permanent_vchildren_edge_colors[perm_idx];
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            if ((*y).level == level) {
                                new_edge_info[i][4].push_back((*y).ranked ? (*y).rank : -1);
                                new_edge_info[i][7].push_back(edge_color);
                            }
                            goto next_father;
                        }
                    }
                    (*new_nodes[i]).vchildren_a.push_back(y);
                    (*new_nodes[i]).vchildren_a_edge_colors.push_back(edge_color);
                    if ((*y).level == level) {
                        new_edge_info[i][5].push_back((*y).ranked ? (*y).rank : -1);
                        new_edge_info[i][8].push_back(edge_color);
                    }
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                // Sort ranks independently
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
                sort_descending_int_vector(new_edge_info[i][6]);
                sort_descending_int_vector(new_edge_info[i][7]);
                sort_descending_int_vector(new_edge_info[i][8]);
            }
            sort_descending_vertex_data(new_edge_info);
            return std::make_tuple(new_edge_info, new_nodes);
        }
        std::tuple
        <
            std::vector<std::array<std::vector<int>, 9>>,
            std::vector<node*>
        > 
        next_depth_resolution_b (std::vector<node*>& nodes, int& level) { // this function can be streamlined by changing the outward edges to nodes so the new_nodes doesn't need to be created each iteration
            std::vector<std::array<std::vector<int>, 9>> new_edge_info = {};
            std::vector<node*> new_nodes = {};
            for (node* x : nodes) {
                for (int edge_idx = 0; edge_idx < (*x).vchildren_b.size(); edge_idx++) {
                    node* y = (*x).vchildren_b[edge_idx];
                    int edge_color = (*x).vchildren_b_edge_colors[edge_idx];
                    for (int i = 0; i < new_nodes.size(); i++) {
                        if (y == new_nodes[i]) {
                            if ((*x).level == level) {
                                new_edge_info[i][3].push_back((*x).ranked ? (*x).rank : -1);
                                new_edge_info[i][6].push_back(edge_color);
                            }
                            break;
                        }
                    }
                    new_nodes.push_back(y);
                    if ((*x).level == level) {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {},
                            {(*x).ranked ? (*x).rank : -1}, 
                            {}, {},
                            {edge_color},
                            {}, {}
                        }});
                    } 
                    else {
                        new_edge_info.push_back({{
                            {}, 
                            {}, {}, {}, {}, {},
                            {}, {}, {}
                        }});
                    }
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                (*new_nodes[i]).vchildren_b = {};
                (*new_nodes[i]).vchildren_b_edge_colors = {};
                for (int perm_idx = 0; perm_idx < (*new_nodes[i]).permanent_vchildren.size(); perm_idx++) {
                    node* y = (*new_nodes[i]).permanent_vchildren[perm_idx];
                    int edge_color = (*new_nodes[i]).permanent_vchildren_edge_colors[perm_idx];
                    for (node* z : nodes) {
                        if (y == z) {
                            goto next_father;
                        }
                    }
                    for (node* z : new_nodes) {
                        if (y == z) {
                            if ((*y).level == level) {
                                new_edge_info[i][4].push_back((*y).ranked ? (*y).rank : -1);
                                new_edge_info[i][7].push_back(edge_color);
                            }
                            goto next_father;
                        }
                    }
                    (*new_nodes[i]).vchildren_b.push_back(y);
                    (*new_nodes[i]).vchildren_b_edge_colors.push_back(edge_color);
                    if ((*y).level == level) {
                        new_edge_info[i][5].push_back((*y).ranked ? (*y).rank : -1);
                        new_edge_info[i][8].push_back(edge_color);
                    }
                    next_father:;
                }
            }
            for (int i = 0; i < new_nodes.size(); i++) {
                // Sort ranks independently
                sort_descending_int_vector(new_edge_info[i][3]);
                sort_descending_int_vector(new_edge_info[i][4]);
                sort_descending_int_vector(new_edge_info[i][5]);
                sort_descending_int_vector(new_edge_info[i][6]);
                sort_descending_int_vector(new_edge_info[i][7]);
                sort_descending_int_vector(new_edge_info[i][8]);
            }
            sort_descending_vertex_data(new_edge_info);
            return std::make_tuple(new_edge_info, new_nodes);
        }
        int conclusive_vertex_comparison (node* a, node* b) {

            (*a).vchildren_a = (*a).permanent_vchildren;
            (*a).vchildren_a_edge_colors = (*a).permanent_vchildren_edge_colors;
            (*b).vchildren_b = (*b).permanent_vchildren;
            (*b).vchildren_b_edge_colors = (*b).permanent_vchildren_edge_colors;
            
            std::vector<int> a_levels = {};
            std::vector<int> a_ranks = {};
            std::vector<int> a_edge_colors = {};
            for (int i = 0; i < (*a).permanent_vchildren.size(); i++) {
                node* x = (*a).permanent_vchildren[i];
                a_levels.push_back((*x).level);
                a_edge_colors.push_back((*a).permanent_vchildren_edge_colors[i]);
                // Use rank -1 for unranked nodes
                a_ranks.push_back((*x).ranked ? (*x).rank : -1);
            }
            std::vector<int> b_levels = {};
            std::vector<int> b_ranks = {};
            std::vector<int> b_edge_colors = {};
            for (int i = 0; i < (*b).permanent_vchildren.size(); i++) {
                node* x = (*b).permanent_vchildren[i];
                b_levels.push_back((*x).level);
                b_edge_colors.push_back((*b).permanent_vchildren_edge_colors[i]);
                // Use rank -1 for unranked nodes
                b_ranks.push_back((*x).ranked ? (*x).rank : -1);
            }

            // Use respect_sort to maintain correspondence between levels/ranks and edge colors
            respect_sort(a_levels, a_edge_colors);
            respect_sort(b_levels, b_edge_colors);
            respect_sort(a_ranks, a_edge_colors);
            respect_sort(b_ranks, b_edge_colors);
            std::vector<std::array<std::vector<int>, 9>> a_vertex_data = {{
                {a_levels, {}, {}, a_ranks, {}, {}, a_edge_colors, {}, {}}
            }};
            std::vector<std::array<std::vector<int>, 9>> b_vertex_data = {{
                {b_levels, {}, {}, b_ranks, {}, {}, b_edge_colors, {}, {}}
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
                std::vector<std::array<std::vector<int>, 9>>,
                std::vector<node*>
            > next_depth_info_a = next_depth_a(a_nodes);
            std::tuple
            <
                std::vector<std::array<std::vector<int>, 9>>,
                std::vector<node*>
            > next_depth_info_b = next_depth_b(b_nodes);
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
        std::function<int(node*, node*)> conclusive_vertex_comparison_obj = [this](node* a, node* b) { return conclusive_vertex_comparison(a, b); };
        int vertex_comparison (node* a, node* b, int& level) {

            (*a).vchildren_a = (*a).permanent_vchildren;
            (*a).vchildren_a_edge_colors = (*a).permanent_vchildren_edge_colors;
            (*b).vchildren_b = (*b).permanent_vchildren;
            (*b).vchildren_b_edge_colors = (*b).permanent_vchildren_edge_colors;
            
            std::vector<int> a_ranks = {};
            std::vector<int> a_edge_colors = {};
            for (int i = 0; i < (*a).permanent_vchildren.size(); i++) {
                node* x = (*a).permanent_vchildren[i];
                if ((*x).level == level) {
                    // Use rank -1 for unranked nodes at this level
                    a_ranks.push_back((*x).ranked ? (*x).rank : -1);
                    a_edge_colors.push_back((*a).permanent_vchildren_edge_colors[i]);
                }
            }
            std::vector<int> b_ranks = {};
            std::vector<int> b_edge_colors = {};
            for (int i = 0; i < (*b).permanent_vchildren.size(); i++) {
                node* x = (*b).permanent_vchildren[i];
                if ((*x).level == level) {
                    // Use rank -1 for unranked nodes at this level
                    b_ranks.push_back((*x).ranked ? (*x).rank : -1);
                    b_edge_colors.push_back((*b).permanent_vchildren_edge_colors[i]);
                }
            }
            
            // Use respect_sort to maintain rank-color correspondence
            respect_sort(a_ranks, a_edge_colors);
            respect_sort(b_ranks, b_edge_colors);
            std::vector<std::array<std::vector<int>, 9>> a_vertex_data = {{
                { {}, {}, {}, a_ranks, {}, {}, a_edge_colors, {}, {} }
            }};
            std::vector<std::array<std::vector<int>, 9>> b_vertex_data = {{
                { {}, {}, {}, b_ranks, {}, {}, b_edge_colors, {}, {} }
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
                std::vector<std::array<std::vector<int>, 9>>,
                std::vector<node*>
            > next_depth_info_a = next_depth_resolution_a(a_nodes, level);
            std::tuple
            <
                std::vector<std::array<std::vector<int>, 9>>,
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
                // Sort nodes by rank for canonical output order
                std::vector<node*> sorted_nodes = nodes_stratified[i];
                std::sort(sorted_nodes.begin(), sorted_nodes.end(), 
                         [](node* a, node* b) { return a->rank < b->rank; });
                
                for (node* n : sorted_nodes) {
                    vectorized.push_back(n->rank);
                    // Create pairs of (rank, color) for stable sorting
                    std::vector<std::pair<int, int>> rank_color_pairs;
                    for (int k = 0; k < n->fathers.size(); k++) {
                        node* x = n->fathers[k];
                        int edge_color = n->fathers_edge_colors[k];
                        rank_color_pairs.push_back({x->rank, edge_color});
                    }
                    // Sort by rank descending, then by color descending for stability
                    std::sort(rank_color_pairs.begin(), rank_color_pairs.end(), 
                             [](const auto& a, const auto& b) {
                                 if (a.first != b.first) return a.first > b.first;
                                 return a.second > b.second;
                             });
                    // Output sorted rank-color pairs
                    for (const auto& pair : rank_color_pairs) {
                        vectorized.push_back(pair.first);
                        vectorized.push_back(pair.second);
                    }
                }
            }
            return vectorized;
        }
};
