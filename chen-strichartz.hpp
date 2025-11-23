#pragma once

#include <cmath>
#include <vector>
#include <array>
#include <utility>
#include <functional>
#include <algorithm>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <tuple>

typedef std::array<int, 3> ce; // colored edge: (from, to, color)
typedef std::vector<ce> bcd; // colored directed graph
typedef std::tuple<double, bcd, int> wbcd; // weighted bicolored directed graph
typedef std::vector<wbcd> swbcd; // sum of weighted bicolored directed graphs

// Function to calculate the binomial coefficient C(n, k)
int binomial(int n, int k) {

    // Use symmetry property: C(n, k) = C(n, n-k)
    // This reduces computations if k > n/2
    if (k > n - k) {
        k = n - k;
    }

    int res = 1; // Initialize result

    // Calculate C(n, k) = n * (n-1) * ... * (n-k+1) / (k * (k-1) * ... * 1)
    for (int i = 0; i < k; ++i) {
        res = res * (n - i) / (i + 1); // Perform multiplication and division in each step
    }

    return res;
}

int descents (std::vector<int> perm) {
     int d = 0;
     for (int i = 0; i < perm.size() - 1; i++) {
         if (perm[i] > perm[i + 1]) {
             d++;
         }
     }
     return d;
}

int factorial_(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial_(n - 1);
}

swbcd leibniz(swbcd original_graphs, int n, int loop_order, bool bicolored = true) {
    swbcd out = {};
    for (auto graph : original_graphs) {
        int highest_multiplicity = loop_order - std::get<2>(graph);
        for (int multiplicity = 1; multiplicity <= highest_multiplicity; multiplicity++) {
            int mathematical_index = std::pow(n - 1, multiplicity);
            for (int i = 0; i < mathematical_index; i++) {
                // converts the mathematical index to a base (n-1) representation
                std::vector<int> base_representation = {};
                int temp_index = i;
                for (int j = 0; j < multiplicity; j++) {
                    base_representation.push_back(temp_index % (n - 1));
                    temp_index /= (n - 1);
                }
                // creates a new graph by adding edges from the base representation
                auto temp_graph1 = std::get<1>(graph);
                for (int j = 0; j < multiplicity; j++) {
                    temp_graph1.push_back({base_representation[j], n - 1, 0});
                }
                out.push_back({std::get<0>(graph) / factorial_(multiplicity - 1), temp_graph1, std::get<2>(graph) + multiplicity - 1}); 
                if (!bicolored) {
                    continue;
                }
                auto temp_graph2 = std::get<1>(graph);
                for (int j = 0; j < multiplicity; j++) {
                    temp_graph2.push_back({base_representation[j], n - 1, 1});                                                                                  }
                out.push_back({-1 * std::get<0>(graph) / factorial_(multiplicity - 1), temp_graph2, std::get<2>(graph) + multiplicity - 1});
            }
        }
    }
    return out;
}



swbcd graph_blueprints (int max, int loop_order = 1, bool pm = true) {
    if (max == 2) {
       swbcd base_case = {};
       for (int i = 0; i < loop_order; i++) {
           bcd graph1 = {};
           for (int j = 0; j <= i; j++) {
                graph1.push_back({0, 1, 0});
           }
           base_case.push_back({1.0 / factorial_(i + 1), graph1, i});
           if (!pm) {
               continue;
           }
           bcd graph2 = {};
           for (int j = 0; j <= i; j++) {
                graph2.push_back({0, 1, 1});
           }
           base_case.push_back({-1.0 / factorial_(i + 1), graph2, i});
       }
       return base_case;
    }
    swbcd out = {};
    swbcd lower = graph_blueprints(max - 1, loop_order, pm);
    swbcd temp = leibniz(
        lower, 
        max, 
        loop_order,
        pm
    );
    out.insert(out.end(), temp.begin(), temp.end());
    return out;
}

swbcd blueprints_to_graphs(swbcd& blueprints, int max) {
    swbcd out = {};
    std::vector<int> indices(max);
    std::iota(indices.begin(), indices.end(), 0);
    double reciprocal_quadratic = max * max;
    // permutes the indices and generates graphs for each permutation using the blueprints
    do {
        int d = descents(indices);
        int sign = (d % 2 == 0) ? 1 : -1;
        double weight = sign / reciprocal_quadratic / binomial(max - 1, d);
        for (auto blueprint : blueprints) {
            bcd temp_graph = {};
            double subsign = std::get<0>(blueprint);
            for (auto edge : std::get<1>(blueprint)) {
                if (indices[edge[0]] > indices[edge[1]]) {
                    subsign *= -1;
                    edge = {edge[1], edge[0], edge[2]};
                }
                temp_graph.push_back({indices[edge[0]], indices[edge[1]], edge[2]});
            }
            out.push_back(std::make_tuple(weight * subsign, temp_graph, std::get<2>(blueprint)));
        }
    } while (std::next_permutation(indices.begin(), indices.end()));
    return out;
}

swbcd cs(int max, int loop_order = 1, bool pm = true) {
    swbcd blueprints = graph_blueprints(max, loop_order, pm);
    swbcd graphs = blueprints_to_graphs(blueprints, max);
    return graphs;
}
