#pragma once

#include <array>
#include <vector>

#include "pograph_node.hpp"

// for faster computation, change layer sizes and layers to lists

std::vector<std::vector<node*>> layers_from_vectorized (std::vector<int> vectorized) {
    int index = 0;
    std::vector<int> layer_sizes = {};
    continue_layer_info:;
    if(vectorized[index] != 0) {
        layer_sizes.push_back(vectorized[index]);
        index++;
        goto continue_layer_info;
    }
    std::vector<std::vector<node*>> layers (layer_sizes.size());
    std::vector<node*> nodes;
    for (int i = 0; i < layer_sizes.size(); i++) {
        layers[i] = std::vector<node*>(layer_sizes[i]);
        for (int j = 0; j < layer_sizes[i]; j++) {
            layers[i][j] = new node();
            nodes.push_back(layers[i][j]);
        }
    }
    continue_node_info:;
    while (index != vectorized.size()) {
        int rank = vectorized[index];
        index++;
        while (vectorized[index] < rank) {
            (*nodes[rank]).fathers.push_back(nodes[vectorized[index]]);
            (*nodes[vectorized[index]]).children.push_back(nodes[rank]);
            index++;
        }
    }
    return layers;
}

int n_compatible_tosets (std::vector<std::vector<node*>> layers) {
    if (layers.size() == 1 and layers[0].size() == 1) {
        return 1;
    }
    int acc = 0;
    for (int i = 0; i < layers.size(); i++) {
        std::vector<std::vector<node*>> new_layers = layers;
        new_layers[0].erase(new_layers[0].begin() + i);
        acc += n_compatible_tosets(new_layers);
    }
    return acc;
}