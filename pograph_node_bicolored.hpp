#pragma once

#include <vector>

struct node {
    public:
        std::vector<node*> fathers = {};
        std::vector<node*> children = {};
        std::vector<int> fathers_edge_colors = {};
        std::vector<int> children_edge_colors = {};
        std::vector<node*> vchildren_a;
        std::vector<int> vchildren_a_edge_colors;
        std::vector<node*> vchildren_b;
        std::vector<int> vchildren_b_edge_colors;
        std::vector<node*> permanent_vchildren = {};
        std::vector<int> permanent_vchildren_edge_colors = {};
        int level = -1;
        bool leveled = false;
        int rank = -1;
        bool ranked = false;
};
