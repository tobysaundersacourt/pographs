// Abacus B-Tree
// description:

#pragma once

#include <vector>
#include <iostream>

// Note: btree also works for substrings, meaning that "2, 3" will be said to be in the tree if "1, 2, 3" is (it stores nodes in the reverse order of the vector)
// Implement btree for arbitrary containers. Also consider if using a list for children_values would be a better choice.
template <typename T, typename T_>
struct btree {
    private:
        T_ value;
        std::vector<T> children_values = {};
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
        void private_add (std::vector<T> v, int size, T_ value) {
            if (size != 0) {
                size--;
                for (int i = 0; i < children_values.size(); i++) {
                    if (children_values[i] == v[size]) {
                        (*children_pointers[i]).private_add(v, size, value);
                        return;
                    }
                }
                btree<T, T_>* btr_ptr = new btree<T, T_>; // need to use custom resource management so children don't get deleted when leaving "update" member function's scope; ADD CUSTOM DESTRUCTOR, AS WELL! CURRENTLY, ONLY ROOT GETS DELETED WHEN SCOPE ITS INITIATED IN ENDS
                children_values.push_back(v[size]);
                children_pointers.push_back(btr_ptr);
                (*btr_ptr).private_add(v, size, value);
                return;
            }
            this->value.insert(this->value.end(), value.begin(), value.end());
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
        void private_print (std::vector<T> v) {
            if (!(value.size() == 0)) {
                std::cout << "[";
                for (int i = 0; i < value.size(); i++) {
                    std::cout << "[";
                    for (int j = 0; j < value[i].size(); j++) {
                        std::cout << value[i][j];
                        if (j != value[i].size() - 1) {
                            std::cout << ",";
                        }
                    }
                    std::cout << "]";
                    if (i != value.size() - 1) {
                        std::cout << ",";
                    }
                }
                std::cout << "]";
                std::cout << " ";
                typename std::vector<T>::reverse_iterator rit;
                for (rit = v.rbegin(); rit != v.rend(); ++rit) {
                    std::cout << *rit << " ";
                }
                std::cout << std::endl;
            }
            if (children_values.size() > 0) {
                v.push_back(children_values[0]);
                (*children_pointers[0]).private_print(v);
                for (int i = 1; i < children_values.size(); i++) {
                    v[v.size() - 1] = children_values[i];
                    (*children_pointers[i]).private_print(v);
                }
            }
        }
    public:
        void update (std::vector<T> v) {
            private_update(v, v.size());
        }
        void add(std::vector<T> v, T_ value) {
            private_add(v, v.size(), value);
        }
        bool contains (std::vector<T> v) {
            return private_contains(v, v.size());
        }
        void print () {
            private_print({});
        }
        std::vector<T> get_children_values () {
            return children_values;
        }
        std::vector<btree<T, T_>*> get_children_pointers () {
            return children_pointers;
        }
        T_ get_value () {
            return value;
        }
};