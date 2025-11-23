// Abacus B-Tree
// description:

#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include <cmath>
#include <array>
#include <map>

// Note: btree also works for substrings, meaning that "2, 3" will be said to be in the tree if "1, 2, 3" is (it stores nodes in the reverse order of the vector)
// Implement btree for arbitrary containers. Also consider if using a list for children_values would be a better choice.
template <typename T, typename T_>
struct btree {
    private:
        std::map<int, T_> polynomial;
        std::vector<T> children_values = {}; // rename -- these are not really "values", but rather "keys" or "edges"
        std::vector<btree<T, T_>*> children_pointers = {};
        void private_update (std::vector<T> v, int size) {
            if (size != 0) {
                size--;
                for (int i = 0; i < children_values.size(); i++) {
                    if (children_values[i] == v[size]) {
                        (*children_pointers[i]).private_update(v, size);
                        return;
                    }
                }
                btree<T, T_>* btr_ptr = new btree<T, T_>; // need to use custom resource management so children don't get deleted when leaving "update" member function's scope; ADD CUSTOM DESTRUCTOR, AS WELL! CURRENTLY, ONLY ROOT GETS DELETED WHEN SCOPE ITS INITIATED IN ENDS
                children_values.push_back(v[size]);
                children_pointers.push_back(btr_ptr);
                (*btr_ptr).private_update(v, size);
            }
        }
        void private_add (std::vector<T> v, int size, T_ coefficient, int degree) {
            if (size != 0) {
                size--;
                for (int i = 0; i < children_values.size(); i++) {
                    if (children_values[i] == v[size]) {
                        (*children_pointers[i]).private_add(v, size, coefficient, degree);
                        return;
                    }
                }
                btree<T, T_>* btr_ptr = new btree<T, T_>; // need to use custom resource management so children don't get deleted when leaving "update" member function's scope; ADD CUSTOM DESTRUCTOR, AS WELL! CURRENTLY, ONLY ROOT GETS DELETED WHEN SCOPE ITS INITIATED IN ENDS
                children_values.push_back(v[size]);
                children_pointers.push_back(btr_ptr);
                (*btr_ptr).private_add(v, size, coefficient, degree);
                return;
            }
            polynomial[degree] += coefficient;
            if (polynomial[degree] == 0) {
                polynomial.erase(degree);
            }
        }
        bool private_contains (std::vector<T> v, int size) {
            if (size != 0) {
                size--;
                for (int i = 0; i < children_values.size(); i++) {
                    if (children_values[i] == v[size]) {
                        return (*children_pointers[i]).private_contains(v, size);
                    }
                }
                return false;
            }
            return true;
        }
        void private_print_nonzero (std::vector<T> v, T_ threshold) {
            if (!polynomial.empty()) {
                bool has_nonzero = false;
                for (const auto& term : polynomial) {
                    if (std::abs(term.second) > threshold) {
                        has_nonzero = true;
                        break;
                    }
                }
                if (has_nonzero) {
                    bool first = true;
                    for (const auto& term : polynomial) {
                        if (!first) {
                            if (term.second > 0) std::cout << " + ";
                            else std::cout << " - ";
                        } else if (term.second < 0) {
                            std::cout << "-";
                        }
                        
                        T_ coeff = std::abs(term.second);
                        if (term.first == 0) {
                            std::cout << coeff;
                        } else if (term.first == 1) {
                            if (coeff == 1) std::cout << "h";
                            else std::cout << coeff << "*h";
                        } else {
                            if (coeff == 1) std::cout << "h^" << term.first;
                            else std::cout << coeff << "*h^" << term.first;
                        }
                        first = false;
                    }
                    std::cout << " at ";
                    typename std::vector<T>::reverse_iterator rit;
                    for (rit = v.rbegin(); rit != v.rend(); ++rit) {
                        std::cout << *rit << " ";
                    }
                    std::cout << std::endl;
                }
            }
            if (children_values.size() > 0) {
                v.push_back(children_values[0]);
                (*children_pointers[0]).private_print_nonzero(v, threshold);
                for (int i = 1; i < children_values.size(); i++) {
                    v[v.size() - 1] = children_values[i];
                    (*children_pointers[i]).private_print_nonzero(v, threshold);
                }
            }
        }
        void private_print_all (std::vector<T> v) {
            if (polynomial.empty()) {
                std::cout << "0";
            } else {
                bool first = true;
                for (const auto& term : polynomial) {
                    if (!first) {
                        if (term.second > 0) std::cout << " + ";
                        else std::cout << " - ";
                    } else if (term.second < 0) {
                        std::cout << "-";
                    }
                    
                    T_ coeff = std::abs(term.second);
                    if (term.first == 0) {
                        std::cout << coeff;
                    } else if (term.first == 1) {
                        if (coeff == 1) std::cout << "h";
                        else std::cout << coeff << "*h";
                    } else {
                        if (coeff == 1) std::cout << "h^" << term.first;
                        else std::cout << coeff << "*h^" << term.first;
                    }
                    first = false;
                }
            }
            std::cout << " at ";
            typename std::vector<T>::reverse_iterator rit;
            for (rit = v.rbegin(); rit != v.rend(); ++rit) {
                std::cout << *rit << " ";
            }
            std::cout << std::endl;
            if (children_values.size() > 0) {
                v.push_back(children_values[0]);
                (*children_pointers[0]).private_print_all(v);
                for (int i = 1; i < children_values.size(); i++) {
                    v[v.size() - 1] = children_values[i];
                    (*children_pointers[i]).private_print_all(v);
                }
            }
        }
    public:
        void update (std::vector<T> v) {
            private_update(v, v.size());
        }
        void add(std::vector<T> v, T_ coefficient, int degree) {
            private_add(v, v.size(), coefficient, degree);
        }
        bool contains (std::vector<T> v) {
            return private_contains(v, v.size());
        }
        void print_nonzero(T_ threshold=0) {
            private_print_nonzero({}, threshold);
        }
        void print_all() {
            private_print_all({});
        }
        std::vector<std::pair<std::vector<T_>, std::vector<T>>> get_weighted_paths (std::vector<T> current_path = {}) {
            std::vector<std::pair<std::vector<T_>, std::vector<T>>> output = {};
            if (!polynomial.empty()) {
                int max_degree = polynomial.rbegin()->first;
                std::vector<T_> coeff_vector(max_degree + 1, 0);
                for (const auto& term : polynomial) {
                    coeff_vector[term.first] = term.second;
                }
                output.push_back({coeff_vector, current_path});
            }
            for (int i = 0; i < children_values.size(); i++) {
                std::vector<T> new_path = current_path;
                new_path.push_back(children_values[i]);
                std::vector<std::pair<std::vector<T_>, std::vector<T>>> child_output = (*children_pointers[i]).get_weighted_paths(new_path);
                output.insert(output.end(), child_output.begin(), child_output.end());
            }
            return output;
        }
        std::vector<T> get_children_values () {
            return children_values;
        }
        std::vector<btree<T, T_>*> get_children_pointers () {
            return children_pointers;
        }
        std::map<int, T_> get_specific_polynomial (std::vector<T> v) {
            if (v.size() == 0) {
                return polynomial;
            } else {
                int size = v.size() - 1;
                for (int i = 0; i < children_values.size(); i++) {
                    if (children_values[i] == v[size]) {
                        std::vector<T> v_new = v;
                        v_new.pop_back();
                        return (*children_pointers[i]).get_specific_polynomial(v_new);
                    }
                }
                return {};
            }
        }
        std::map<int, T_> get_polynomial () {
            return polynomial;
        }
};
