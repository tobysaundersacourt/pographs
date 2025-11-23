#pragma once

#include "pograph_bicolored.hpp"
#include <map>
#include <vector>

// Forward declaration
pograph create_pograph_with_node_removed(const pograph& original_pg, node* node_to_remove);

// Helper function to calculate factorial
long long factorial(int n) {
    if (n <= 1) return 1;
    long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Function to count linear extensions of a partially ordered graph (pograph)
// Algorithm: Recursive counting by removing nodes from lowest stratification level (minimal elements)
long long count_linear_extensions(const pograph& pg) {
    // Base case: if pograph has one node or less, return 1
    if (pg.n_nodes <= 1) {
        return 1;
    }
    
    // Special case: if all nodes are at level 0 (no dependencies), return n!
    if (pg.max_level == 0) {
        return factorial(pg.n_nodes);
    }
    
    // Initialize accumulator
    long long accumulator = 0;
    
    // Get nodes from the LOWEST stratification level (these are minimal elements with no fathers)
    int lowest_level = 0;
    const std::vector<node*>& minimal_level_nodes = pg.nodes_stratified.at(lowest_level);
    
    // For each node in the minimal stratification level
    for (int i = 0; i < minimal_level_nodes.size(); i++) {
        node* node_to_remove = minimal_level_nodes[i];
        
        // Create a new pograph with this node removed
        pograph new_pg = create_pograph_with_node_removed(pg, node_to_remove);
        
        // Recursively count linear extensions of the new pograph
        accumulator += count_linear_extensions(new_pg);
    }
    
    return accumulator;
}

// Helper function to create a new pograph with a specific node removed
pograph create_pograph_with_node_removed(const pograph& original_pg, node* node_to_remove) {
    // Create empty pograph
    pograph new_pg;
    
    // Collect remaining node labels
    std::vector<int> remaining_labels;
    for (int i = 0; i < original_pg.nodes.size(); i++) {
        if (original_pg.nodes[i] != node_to_remove) {
            remaining_labels.push_back(original_pg.labels[i]);
        }
    }
    
    // If no nodes remain, return empty pograph
    if (remaining_labels.empty()) {
        return new_pg;
    }
    
    // Create a new set of inequalities excluding edges involving the removed node
    std::vector<std::array<int, 3>> new_inequalities;
    
    // Reconstruct inequalities from the original graph, excluding the removed node
    for (int i = 0; i < original_pg.nodes.size(); i++) {
        node* current_node = original_pg.nodes[i];
        int current_label = original_pg.labels[i];
        
        // Skip the node we're removing
        if (current_node == node_to_remove) {
            continue;
        }
        
        // Add edges to fathers (current_node < father)
        for (node* father : current_node->fathers) {
            // Find father's label and ensure father is not the removed node
            for (int j = 0; j < original_pg.nodes.size(); j++) {
                if (original_pg.nodes[j] == father && father != node_to_remove) {
                    int father_label = original_pg.labels[j];
                    new_inequalities.push_back({current_label, father_label, 0}); // crucial! we're not using color to compute linear extensions!
                    break;
                }
            }
        }
    }
    
    // If we have remaining nodes but no inequalities, we need to handle disconnected nodes
    if (new_inequalities.empty() && !remaining_labels.empty()) {
        // For disconnected nodes, we can't use read_inequalities since there are no inequalities
        // Instead, we'll manually set up the pograph structure
        new_pg.n_nodes = remaining_labels.size();
        new_pg.max_level = 0; // All nodes are at level 0 (no dependencies)
        new_pg.labels = remaining_labels;
        
        // Create nodes and add them to level 0
        for (int label : remaining_labels) {
            node* new_node = new node();
            new_node->level = 0;
            new_node->leveled = true;
            new_pg.nodes.push_back(new_node);
            new_pg.nodes_stratified[0].push_back(new_node);
        }
        
        return new_pg;
    }
    
    // Build the new pograph from the filtered inequalities
    new_pg.read_inequalities(new_inequalities);
    
    // Add any isolated nodes that weren't included in the inequalities
    for (int label : remaining_labels) {
        bool found = false;
        for (int i = 0; i < new_pg.labels.size(); i++) {
            if (new_pg.labels[i] == label) {
                found = true;
                break;
            }
        }
        
        // If this label wasn't included, add it as an isolated node at level 0
        if (!found) {
            node* isolated_node = new node();
            isolated_node->level = 0;
            isolated_node->leveled = true;
            new_pg.nodes.push_back(isolated_node);
            new_pg.labels.push_back(label);
            new_pg.nodes_stratified[0].push_back(isolated_node);
            new_pg.n_nodes++;
        }
    }
    
    return new_pg;
}
