#pragma once

#include <list>
#include <vector>
#include <algorithm>

#include "comparisons.hpp"
#include "pograph_node.hpp"

// need to edit rank sort so current_class is iterator to vector of vector of node* iterators

void rank_sort (
    std::vector<node*>::iterator begin, 
    std::vector<node*>::iterator end,
    std::function<int(node*, node*, int&)> comp_func,
    int& cumulative_rank,
    int& level
) {
    if (begin == end - 1) {
        (*begin)->rank = cumulative_rank;
        (*begin)->ranked = true;
        cumulative_rank++;
        return;
    }
    else if (begin == end - 2) {
        int comp = comp_func(*begin, *(end - 1), level);
        if (comp == 1) {
            std::iter_swap(begin, end - 1);
        }
        (*begin)->rank = cumulative_rank;
        (*begin)->ranked = true;
        cumulative_rank++;
        (*(end - 1))->rank = cumulative_rank;
        (*(end - 1))->ranked = true;
        cumulative_rank++;
        return;
    }
    std::list<std::vector<node*>::iterator> beginnings = {begin};
    std::list<std::vector<node*>::iterator> endings = {end};
    std::vector<std::vector<node*>::iterator> transitions(end - begin, end);
    int comp;
    beginning_of_loop:;
    if (beginnings.empty()) {
        std::vector<std::vector<node*>::iterator>::iterator it = transitions.begin(); // the iterator points to the vector of iterators to the sorted elements
        std::vector<std::vector<node*>::iterator>::iterator current_class_pointer = it; // get the pointer to the iterator of the first element in the current equivalence class
        (**current_class_pointer)->rank = cumulative_rank;
        (**current_class_pointer)->ranked = true;
        cumulative_rank++;
        it++;
        while (it != transitions.end()) {
            if (*it != end) {
                if (!(**current_class_pointer)->ranked) {
                    if (*it - *current_class_pointer > 1) {
                        rank_sort(*current_class_pointer, *it, comp_func, cumulative_rank, level);
                    }
                    else {
                        (**current_class_pointer)->rank = cumulative_rank;
                        (**current_class_pointer)->ranked = true;
                        cumulative_rank++;
                    }
                }
                else {
                    if (*it - *current_class_pointer > 2) {
                        rank_sort(*current_class_pointer + 1, *it, comp_func, cumulative_rank, level);
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
            if (transitions.end() - current_class_pointer > 1) {
                rank_sort(*current_class_pointer, end, comp_func, cumulative_rank, level);
            }
            else {
                (**current_class_pointer)->rank = cumulative_rank;
                (**current_class_pointer)->ranked = true;
                cumulative_rank++;
            }
        }
        else {
            if (transitions.end() - current_class_pointer > 2) {
                rank_sort(*current_class_pointer + 1, end, comp_func, cumulative_rank, level);
            }
            else if (transitions.end() - current_class_pointer == 2) {
                (*(*current_class_pointer + 1))->rank = cumulative_rank;
                (*(*current_class_pointer + 1))->ranked = true;
                cumulative_rank++;
            }
        }
        return;
    }
    std::vector<node*>::iterator b = beginnings.front();
    std::vector<node*>::iterator e = endings.front();
    std::list<std::vector<node*>::iterator> high_equivalences = {};
    std::list<std::vector<node*>::iterator> low_equivalences = {};
    beginnings.pop_front();
    endings.pop_front();
    std::vector<node*>::iterator low = b + 1;
    std::vector<node*>::iterator high = e - 1;
    int size = high - low;
    if (size == -2) {
        goto beginning_of_loop;
    }
    else if (size == -1) {
        transitions[b - begin] = b;
        goto beginning_of_loop;
    }
    else if (size == 0) {
        comp = comp_func(*b, *(b + 1), level);
        if (comp == 1) {
            std::iter_swap(b, b + 1);
            transitions[b - begin] = b;
            transitions[b + 1 - begin] = b + 1;
        }
        else if (comp == 2) {
            transitions[b - begin] = b;
            transitions[b + 1 - begin] = b + 1;
        }
        else {
            transitions[b - begin] = b;
        }
        goto beginning_of_loop;
    }
    node* pivot = *b;
    continue_with_current_pivot:;
    continue_high_search:;
    comp = comp_func(*high, pivot, level);
    if (comp == 0) {
        high_equivalences.push_back(high);
        high--;
        if (b == high) {
            goto terminate_hoare_partition_none_swapped;
        }
        goto continue_high_search;
    }
    else if (comp == 1) {
        high--;
        if (b == high) {
            goto terminate_hoare_partition_none_swapped;
        }
        goto continue_high_search;
    }
    continue_low_search:;
    if (low == high) {
        goto terminate_hoare_partition;
    }
    comp = comp_func(pivot, *low, level);
    if (comp == 0) {
        low_equivalences.push_back(low);
        low++;
        goto continue_low_search;
    }
    else if (comp == 1) {
        low++;
        goto continue_low_search;
    }
    if (low != high) {
        std::iter_swap(low, high);
        high--;
        if (high == low) {
            goto terminate_hoare_partition;
        }
        goto continue_with_current_pivot;
    }
    else {
        terminate_hoare_partition:;
        std::iter_swap(b, high);
        if (!high_equivalences.empty()) {
            std::list<std::vector<node*>::iterator>::reverse_iterator high_equivalences_it = high_equivalences.rbegin();
            do {
                high++;
                std::iter_swap(high, *high_equivalences_it);
                high_equivalences_it++;
            }
            while (high_equivalences_it != high_equivalences.rend());
        }
        if (!low_equivalences.empty()) {
            std::list<std::vector<node*>::iterator>::reverse_iterator low_equivalences_it = low_equivalences.rbegin();
            do {
                low--;
                std::iter_swap(low, *low_equivalences_it);
                low_equivalences_it++;
            }
            while (low_equivalences_it != low_equivalences.rend());
        }
        if (low > b) {
            beginnings.push_back(b);
            endings.push_back(low);
        }
        if (high < e) {
            beginnings.push_back(high + 1);
            endings.push_back(e);
        }
        transitions[low - begin] = low;
    }
    goto beginning_of_loop;
    terminate_hoare_partition_none_swapped:;
    if (!high_equivalences.empty()) {
        std::list<std::vector<node*>::iterator>::reverse_iterator high_equivalences_it = high_equivalences.rbegin();
        do {
            high++;
            std::iter_swap(high, *high_equivalences_it);
            high_equivalences_it++;
        }
        while (high_equivalences_it != high_equivalences.rend());
    }
    if (high < e) {
        beginnings.push_back(high + 1);
        endings.push_back(e);
    }
    transitions[b - begin] = b;
    goto beginning_of_loop;
}
