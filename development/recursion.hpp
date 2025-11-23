#pragma once

#include <vector>
#include <array>
#include <utility>
#include <functional>
#include <algorithm>
#include <iostream>

// can make this program faster by tracking maximum node values as magnus computation progresses
// might also make it faster by neglecting all terms with odd(but not unity)-indexed bernoulli numbers, as these are zero
// except for B_1 = 1/2

int nodes_in_pograph (std::vector<std::array<int, 2>>& inequalities) {
    int max = 0;
    for (auto pair : inequalities) {
        if (pair[0] > max) {
            max = pair[0];
        }
        if (pair[1] > max) {
            max = pair[1];
        }
    }
    return max;
}

std::vector<std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>>> magnus_cache = {
    std::make_pair(
            std::vector<std::vector<std::array<int, 2>>>{ { {2, 1} } },
            std::vector<std::vector<int>>{ 1 }
    )
};
std::vector<std::vector<std::array<int, 2>>> product_leibniz (
    std::vector<std::array<int, 2>>& inequalities1, 
    std::vector<std::array<int, 2>>& inequalities2, 
    int n_nodes1, 
    int n_nodes2
) {
    std::vector<std::array<int, 2>> cup = inequalities1;
    cup.insert(cup.end(), inequalities2.begin(), inequalities2.end());
    std::vector<std::vector<std::array<int, 2>>> out(n_nodes1 + n_nodes2, cup);
    for (int i = 0; i < n_nodes1; i++) {
        for (int j = 0; j < n_nodes2; j++) {
            out[i].push_back({i + 1, j + n_nodes1 + 1});
        }
    }
    return out;
}

std::vector<std::vector<std::array<int, 2>>> zero_leibniz (std::vector<std::array<int, 2>>& inequalities, int n_nodes) {
    std::vector<std::vector<std::array<int, 2>>> out(n_nodes, inequalities);
    for (int i = 0; i < n_nodes; i++) {
        out[i].push_back({i + 1, n_nodes + 1});
    } 
    return out;
}

std::vector<std::vector<std::array<int, 2>>> max_leibniz (std::vector<std::array<int, 2>>& inequalities, int n_nodes) {
    std::vector<std::vector<std::array<int, 2>>> out(n_nodes, inequalities);
    for (int i = 0; i < n_nodes; i++) {
        out[i].push_back({n_nodes + 1, i + 1});
    } 
    return out;
}

std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> graded_inequalities_product_leibniz (
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> gr_ineq_s_1, 
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> gr_ineq_s_2
) {
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> gr_ineq_s = std::make_pair(
        std::vector<std::vector<std::array<int, 2>>>(), std::vector<std::vector<int>>()
    );
    for (int i = 0; i < gr_ineq_s_1.first.size(); i++) {
        for (int j = 0; j < gr_ineq_s_2.first.size(); j++) {
            std::vector<std::vector<std::array<int, 2>>> new_inequalities = product_leibniz(
                gr_ineq_s_1.first[i], 
                gr_ineq_s_2.first[j], 
                nodes_in_pograph(gr_ineq_s_1.first[i]), 
                nodes_in_pograph(gr_ineq_s_2.first[j])
            );
            std::vector<int> new_signs = gr_ineq_s_1.second[i];
            new_signs.insert(new_signs.end(), gr_ineq_s_2.second[j].begin(), gr_ineq_s_2.second[j].end());
            for (std::vector<std::array<int, 2>>& new_inequality : new_inequalities) {
                gr_ineq_s.first.push_back(new_inequality);
                gr_ineq_s.second.push_back(new_signs);
            }
        } 
    }
    return gr_ineq_s;
}

std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> graded_inequalities_zero_leibniz (
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> gr_ineq_s
) {
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> gr_ineq_s_ = std::make_pair(
        std::vector<std::vector<std::array<int, 2>>>(), std::vector<std::vector<int>>()
    );
    for (int i = 0; i < gr_ineq_s.first.size(); i++) {
        int nodes = nodes_in_pograph(gr_ineq_s.first[i]);
        std::vector<std::vector<std::array<int, 2>>> zeroed = zero_leibniz(
            gr_ineq_s.first[i], 
            nodes
        );
        for (int j = 0; j < nodes; j++) {
            gr_ineq_s_.first.push_back(zeroed[j]);
            gr_ineq_s_.second.push_back(gr_ineq_s.second[i]);
        }
    }
    return gr_ineq_s_;
}

std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> graded_inequalities_max_leibniz (
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> gr_ineq_s
) {
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> gr_ineq_s_ = std::make_pair(
        std::vector<std::vector<std::array<int, 2>>>(), std::vector<std::vector<int>>()
    );
    for (int i = 0; i < gr_ineq_s.first.size(); i++) {
        int nodes = nodes_in_pograph(gr_ineq_s.first[i]);
        std::vector<std::vector<std::array<int, 2>>> zeroed = max_leibniz(
            gr_ineq_s.first[i], 
            nodes
        );
        for (int j = 0; j < nodes; j++) {
            gr_ineq_s_.first.push_back(zeroed[j]);
            gr_ineq_s_.second.push_back(gr_ineq_s.second[i]);
        }
    }
    return gr_ineq_s_;
}

void partition_insert (
    std::vector<int> partition, 
    int sum,
    int acc,
    std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>>& term
) {
    if (sum == 0) {
        std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> computed;
        if (partition[0] == 1) {
            computed = std::make_pair(
                std::vector<std::vector<std::array<int, 2>>>{ { {2, 1} } },
                std::vector<std::vector<int>>{ {acc} }
            );
        }
        else {
            computed = graded_inequalities_zero_leibniz(
                magnus_cache[partition[0] - 2]
            );
            for (int i = 0; i < computed.first.size(); i++) {
                computed.second[i].push_back(acc);
            }
        }
        for (int i = 1; i < partition.size(); i++) {
            if (partition[i] == 1) {
                computed = graded_inequalities_max_leibniz(
                    computed
                );
            }
            else {
                computed = graded_inequalities_product_leibniz(
                    magnus_cache[partition[i] - 2],   
                    computed
                );
            }
        }
        term.first.insert(term.first.end(), computed.first.begin(), computed.first.end());
        term.second.insert(term.second.end(), computed.second.begin(), computed.second.end());
    }
    for (int i = 1; i <= sum; i++) {
        std::vector<int> new_partition = partition;
        new_partition.push_back(i);
        partition_insert(new_partition, sum - i, acc + 1, term);
    }
}

std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> magnus (int order) {
    if (order < 2) {
        std::cout << "ERROR: Magnus expansion function is only valid for 2 or more vertices!" << std::endl;
        exit(0);
    }
    for (int j = magnus_cache.size() + 1; j < order; j++) {
        std::pair<std::vector<std::vector<std::array<int, 2>>>, std::vector<std::vector<int>>> term = std::make_pair(
            std::vector<std::vector<std::array<int, 2>>>{},
            std::vector<std::vector<int>>{}
        );
        partition_insert(std::vector<int>{}, j, 1, term);
        magnus_cache.push_back(term);
    }
    return magnus_cache[order - 2];
}