#pragma once

#include <list>
#include <vector>
#include <algorithm>

#include "comparisons.hpp"

int conclusive_int_comparison(int a, int b) {
    if (a < b) {
        return 2;
    }
    else if (a > b) {
        return 1;
    }
    return 0;
}

std::vector<std::vector<int>::iterator> dutch_sort (std::vector<int>::iterator begin, std::vector<int>::iterator end) {
    if (begin == end - 1) {
        return {begin};
    }
    else if (begin == end - 2) {
        if (*begin > *(end - 1)) {
            std::iter_swap(begin, end - 1);
            return {begin, end - 1};
        }
        else if (*begin < *(end - 1)) {
            return {begin, end - 1};
        }
        else {
            return {begin, end};
        }
    }
    std::list<std::vector<int>::iterator> beginnings = {begin};
    std::list<std::vector<int>::iterator> endings = {end};
    std::vector<std::vector<int>::iterator> transitions(end - begin, end);
    int comp;
    beginning_of_loop:;
    if (beginnings.empty()) {
        return transitions;
    }
    std::vector<int>::iterator b = beginnings.front();
    std::vector<int>::iterator e = endings.front();
    std::list<std::vector<int>::iterator> high_equivalences = {};
    std::list<std::vector<int>::iterator> low_equivalences = {};
    beginnings.pop_front();
    endings.pop_front();
    std::vector<int>::iterator low = b + 1;
    std::vector<int>::iterator high = e - 1;
    int size = high - low;
    if (size == -2) {
        goto beginning_of_loop;
    }
    else if (size == -1) {
        transitions[b - begin] = b;
        goto beginning_of_loop;
    }
    else if (size == 0) {
        if (*b > *(b + 1)) {
            std::iter_swap(b, b + 1);
            transitions[b - begin] = b;
            transitions[b + 1 - begin] = b + 1;
        }
        else if (*b < *(b + 1)) {
            transitions[b - begin] = b;
            transitions[b + 1 - begin] = b + 1;
        }
        else {
            transitions[b - begin] = b;
        }
        goto beginning_of_loop;
    }
    int pivot = *b;
    continue_with_current_pivot:;
    continue_high_search:;
    comp = conclusive_int_comparison(*high, pivot);
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
    comp = conclusive_int_comparison(pivot, *low);
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
            std::list<std::vector<int>::iterator>::reverse_iterator high_equivalences_it = high_equivalences.rbegin();
            do {
                high++;
                std::iter_swap(high, *high_equivalences_it);
                high_equivalences_it++;
            }
            while (high_equivalences_it != high_equivalences.rend());
        }
        if (!low_equivalences.empty()) {
            std::list<std::vector<int>::iterator>::reverse_iterator low_equivalences_it = low_equivalences.rbegin();
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
        std::list<std::vector<int>::iterator>::reverse_iterator high_equivalences_it = high_equivalences.rbegin();
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