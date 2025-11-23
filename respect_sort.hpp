#pragma once
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>

// Helper function to sort specified columns within each row while maintaining correspondence
// This sorts the elements within the specified columns of each array/row
template<typename Container2D>
void respect_sort_columns(Container2D& data, std::vector<std::size_t> column_indices) {
    if (data.size() == 0) return;
    
    // For each row (vertex), sort the specified columns together
    for (auto& row : data) {
        if (row.size() == 0) continue;
        
        // Validate column indices
        for (auto col : column_indices) {
            if (col >= row.size()) {
                throw std::invalid_argument("Column index out of bounds");
            }
        }
        
        // Find the size of the arrays we're sorting
        // All specified columns should have the same size
        std::size_t array_size = row[column_indices[0]].size();
        for (auto col : column_indices) {
            if (row[col].size() != array_size) {
                throw std::invalid_argument("All specified columns must have the same size");
            }
        }
        
        if (array_size == 0) continue;
        
        // Create index array for sorting elements within the arrays
        std::vector<std::size_t> indices(array_size);
        std::iota(indices.begin(), indices.end(), 0);
        
        // Sort indices lexicographically by the values in the specified columns
        // Use stable_sort to preserve original order when elements are equal
        std::stable_sort(indices.begin(), indices.end(), [&](std::size_t a, std::size_t b) {
            // Compare lexicographically across the specified columns
            for (auto col : column_indices) {
                if (row[col][a] != row[col][b]) {
                    return row[col][a] > row[col][b]; // Sort in descending order
                }
            }
            return false; // Equal elements, preserve original order
        });
        
        // Apply the permutation to all specified columns
        for (auto col : column_indices) {
            auto temp_array = row[col];
            for (std::size_t i = 0; i < array_size; ++i) {
                row[col][i] = temp_array[indices[i]];
            }
        }
    }
}

// Sort two arrays together lexicographically (first by first array, then by second)
// Maintains correspondence between elements at the same indices
template<typename T, typename T_>
void respect_sort(std::vector<T>& first, std::vector<T_>& second) {
    if (first.size() != second.size()) {
        throw std::invalid_argument("Vectors must have the same size");
    }
    
    if (first.size() == 0) return;
    
    std::vector<std::size_t> indices(first.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    // Use stable_sort to preserve original order when elements are equal
    std::stable_sort(indices.begin(), indices.end(), [&](std::size_t a, std::size_t b) {
        // Sort lexicographically: first by first array, then by second array
        if (first[a] != first[b]) {
            return first[a] > first[b]; // Sort in descending order
        }
        return second[a] > second[b]; // If first elements are equal, sort by second array
    });
    
    // Apply permutation to both arrays
    std::vector<T> temp_first(first.size());
    std::vector<T_> temp_second(second.size());
    
    for (std::size_t i = 0; i < indices.size(); ++i) {
        temp_first[i] = first[indices[i]];
        temp_second[i] = second[indices[i]];
    }
    
    first = std::move(temp_first);
    second = std::move(temp_second);
}

template<typename Container>
void apply_permutation(Container& container, const std::vector<std::size_t>& indices) {
    using ValueType = typename Container::value_type;
    std::vector<ValueType> temp(container.size());
    for (std::size_t i = 0; i < indices.size(); ++i) {
        temp[i] = container[indices[i]];
    }
    for (std::size_t i = 0; i < container.size(); ++i) {
        container[i] = std::move(temp[i]);
    }
}

// Compare elements at given indices across multiple containers lexicographically
template<typename Container, typename... Rest>
bool compare_at_indices(std::size_t a, std::size_t b, const Container& first, const Rest&... rest) {
    if (first[a] != first[b]) {
        return first[a] > first[b]; // Sort in descending order
    }
    if constexpr (sizeof...(rest) > 0) {
        return compare_at_indices(a, b, rest...);
    } else {
        return false; // Equal elements, preserve original order
    }
}

// Sort multiple containers together lexicographically
// Sorts by first container, then second, etc., maintaining correspondence
template<typename... Containers>
void respect_sort_n(Containers&... containers) {
    static_assert(sizeof...(Containers) >= 2, "Must provide at least 2 containers");
    
    std::vector<std::size_t> sizes = {containers.size()...};
    
    // Validate that all containers have the same size
    for (std::size_t i = 1; i < sizes.size(); ++i) {
        if (sizes[i] != sizes[0]) {
            throw std::invalid_argument("All containers must have the same size");
        }
    }
    
    if (sizes[0] == 0) return;
    
    std::vector<std::size_t> indices(sizes[0]);
    std::iota(indices.begin(), indices.end(), 0);
    
    // Use stable_sort to preserve original order when elements are equal
    std::stable_sort(indices.begin(), indices.end(), [&](std::size_t a, std::size_t b) {
        return compare_at_indices(a, b, containers...);
    });
    
    // Apply permutation to all containers
    (apply_permutation(containers, indices), ...);
}
