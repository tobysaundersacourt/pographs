#pragma once

#include <vector>

struct node {
    public:
        std::vector<node*> fathers = {};
        std::vector<node*> children = {};
        std::vector<node*> vchildren_a;
        std::vector<node*> vchildren_b;
        std::vector<node*> permanent_vchildren = {};
        int level = -1;
        bool leveled = false;
        int rank = -1;
        bool ranked = false;
};