#include "pograph_bicolored.hpp"
#include "comparisons.hpp"

#include <iostream>
#include <algorithm>

std::vector<int> vectorization(std::vector<std::array<int, 3>> inequalities) {
    pograph graph;
    graph.read_inequalities(inequalities);
    std::vector<int> vectorized = graph.vectorization();
    return vectorized;
}

void print_vectorization_from_inequalities(std::vector<std::array<int, 3>> inequalities) {
    pograph graph;
    graph.read_inequalities(inequalities);
    std::vector<int> vectorized = graph.vectorization();
    for (int value : vectorized) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

void label_permutation_test(int n_nodes, std::vector<std::array<int, 3>> inequality_indices) {
    std::cout << "Testing Label Permutations..." << std::endl;
    std::vector<int> labels = {};
    for (int i = 0; i < n_nodes; i++) {
        labels.push_back(i + 1);
    }
    std::vector<std::array<int, 3>> permuted_inequalities = {};
    for (auto x : inequality_indices) {
        permuted_inequalities.push_back({labels[x[0] - 1], labels[x[1] - 1], x[2]}); // Keep colors the same
    }
    std::cout << "Original Inequalities: ";
    for (auto x : permuted_inequalities) {
        std::cout << "(" << x[0] << ", " << x[1] << ", " << x[2] << ") ";
    }
    std::vector<int> base_vectorization = vectorization(permuted_inequalities);
    std::cout << "\nBase Vectorization: ";
    for (int value : base_vectorization) {
        std::cout << value << " ";
    }
    while (std::next_permutation(labels.begin(), labels.end())) {
        permuted_inequalities = {};
        for (auto x : inequality_indices) {
            permuted_inequalities.push_back({labels[x[0] - 1], labels[x[1] - 1], x[2]}); // Keep colors the same
        }
        std::vector<int> neo_vectorization = vectorization(permuted_inequalities);
        int status = conclusive_greater(base_vectorization, neo_vectorization);
        if (status != 0) {
            std::cout << "\n\033[1;31mAlert! Label permutation changed vectorization.\033[0m\n";
            std::cout << "New Inequalities: ";
            for (auto x : permuted_inequalities) {
                std::cout << "(" << x[0] << ", " << x[1] << ", " << x[2] << ") ";
            }
            std::cout << "\nNew Vectorization: ";
            for (int value : neo_vectorization) {
                std::cout << value << " ";
            }
            std::cout << "\n\n";
            return;
        } 
    }
    std::cout << "\n\n\033[1;32mNo change in vectorization.\n\033[0m\n" << std::endl;
}

void inequality_permutation_test(std::vector<std::array<int, 3>> inequalities) {
    std::cout << "Testing Inequality Permutations..." << std::endl;
    std::cout << "Original Inequalities: ";
    for (auto x : inequalities) {
        std::cout << "(" << x[0] << ", " << x[1] << ", " << x[2] << ") ";
    }
    std::vector<int> base_vectorization = vectorization(inequalities);
    std::cout << "\nBase Vectorization: ";
    for (int value : base_vectorization) {
        std::cout << value << " ";
    }
    while (std::next_permutation(inequalities.begin(), inequalities.end())) {
        std::vector<int> neo_vectorization = vectorization(inequalities);
        int status = conclusive_greater(base_vectorization, neo_vectorization);
        if (status != 0) {
            std::cout << "\n\n\033[1;31mAlert! Inequality permutation changed vectorization.\n\033[0m\n";
            std::cout << "New Inequalities: ";
            for (auto x : inequalities) {
                std::cout << "(" << x[0] << ", " << x[1] << ", " << x[2] << ") ";
            }
            std::cout << "\nNew Vectorization: ";
            for (int value : neo_vectorization) {
                std::cout << value << " ";
            }
            std::cout << "\n\n";
            return;
        } 
    }
    std::cout << "\n\n\033[1;32mNo change in vectorization.\n\033[0m\n" << std::endl;
}

int main () {

    std::vector<std::vector<std::array<int, 3>>> posets{
        {{1,2,0}},
        {{1,2,1}},
        {{1,2,0},{2,3,1}},
        {{1,2,1},{2,3,0}},
        {{1,3,0},{2,3,1}},
        {{1,3,1},{2,3,0}},
        {{1,2,0},{1,3,1},{2,4,0},{3,4,1}},
        {{1,2,1},{1,3,0},{2,4,1},{3,4,0}},
        {{1,2,0},{3,4,1}},
        {{1,2,1},{3,4,0}},
        {{1,4,0},{1,5,1},{2,4,0},{2,6,1},{3,5,0},{3,6,1}},
        {{1,4,1},{1,5,0},{2,4,1},{2,6,0},{3,5,1},{3,6,0}},
        {{1,2,0},{2,3,1},{1,4,0}},
        {{1,2,1},{2,3,0},{1,4,1}},
        {{1,2,0},{2,3,1},{3,4,0}},
        {{1,2,1},{2,3,0},{3,4,1}},
        {{1,2,0},{1,3,1},{1,4,0},{2,5,1},{3,5,0}},
        {{1,2,1},{1,3,0},{1,4,1},{2,5,0},{3,5,1}},
        {{1,2,0},{1,3,1},{3,4,0},{2,4,1}},
        {{1,2,1},{1,3,0},{3,4,1},{2,4,0}},
        {{1,2,0},{1,3,1},{2,4,0},{3,4,1},{4,5,0}},
        {{1,2,1},{1,3,0},{2,4,1},{3,4,0},{4,5,1}},
        {{1,2,0},{3,4,1},{5,6,0}},
        {{1,2,1},{3,4,0},{5,6,1}},
        {{1,2,0},{2,3,1},{4,5,0},{5,6,1}},
        {{1,2,1},{2,3,0},{4,5,1},{5,6,0}},
        {{1,2,0},{1,3,1},{2,4,0},{3,5,1},{4,6,0},{5,6,1}},
        {{1,2,1},{1,3,0},{2,4,1},{3,5,0},{4,6,1},{5,6,0}},
        {{1,2,0},{2,3,1},{3,4,0},{1,5,1},{5,6,0}},
        {{1,2,1},{2,3,0},{3,4,1},{1,5,0},{5,6,1}},
        {{1,2,0},{2,3,1},{1,4,0},{4,5,1},{3,6,0}},
        {{1,2,1},{2,3,0},{1,4,1},{4,5,0},{3,6,1}},
        {{1,2,0},{1,3,1},{2,4,0},{3,4,1},{1,5,0},{5,6,1}},
        {{1,2,1},{1,3,0},{2,4,1},{3,4,0},{1,5,1},{5,6,0}},
        {{1,2,0},{2,3,1},{3,4,0},{4,5,1},{1,6,0},{6,7,1}},
        {{1,2,1},{2,3,0},{3,4,1},{4,5,0},{1,6,1},{6,7,0}},
    };
    for (auto poset : posets) {
        inequality_permutation_test(poset);
        int max = 1;
        for (auto triple : poset) {
            if (triple[0] > max) {
                max = triple[0];
            }
            if (triple[1] > max) {
                max = triple[1];
            }
        }
        label_permutation_test(max, poset);
    }
}
