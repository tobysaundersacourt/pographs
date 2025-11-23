#include <iostream>
#include <vector>
#include <array>
#include "respect_sort.hpp"

void print_arrays(const std::vector<int>& arr1, const std::vector<int>& arr2, const std::vector<int>& arr3, const std::string& label) {
    std::cout << label << ": [";
    for (int i = 0; i < arr1.size(); i++) std::cout << arr1[i] << (i < arr1.size()-1 ? ", " : "");
    std::cout << "], [";
    for (int i = 0; i < arr2.size(); i++) std::cout << arr2[i] << (i < arr2.size()-1 ? ", " : "");
    std::cout << "], [";
    for (int i = 0; i < arr3.size(); i++) std::cout << arr3[i] << (i < arr3.size()-1 ? ", " : "");
    std::cout << "]" << std::endl;
}

int main() {
    std::cout << "=== Simple respect_sort_columns Test ===\n" << std::endl;
    
    // Test data similar to your debugging output
    // Create a vector with one element (one vertex) containing 9 arrays
    std::vector<std::array<std::vector<int>, 9>> test_data = {
        {{
            {0},        // backward_levels[0]
            {},         // internal_levels[1] - empty
            {2, 0},     // outward_levels[2] - this should get sorted
            {-1},       // backward_ranks[3]
            {},         // internal_ranks[4] - empty  
            {-1, -1},   // outward_ranks[5] - should stay with levels
            {0},        // backward_colors[6]
            {},         // internal_colors[7] - empty
            {0, 0}      // outward_colors[8] - should stay with levels
        }}
    };
    
    std::cout << "Original data:" << std::endl;
    print_arrays(test_data[0][2], test_data[0][5], test_data[0][8], "outward");
    
    std::cout << "\nApplying respect_sort_columns with indices {2, 5, 8}" << std::endl;
    std::cout << "(This should sort outward_levels[2], outward_ranks[5], and outward_colors[8] together)" << std::endl;
    
    // Apply the sort
    respect_sort_columns(test_data, {2, 5, 8});
    
    std::cout << "\nAfter sorting:" << std::endl;
    print_arrays(test_data[0][2], test_data[0][5], test_data[0][8], "outward");
    
    std::cout << "\nExpected result: [2, 0] should become [2, 0] (already in descending order)" << std::endl;
    std::cout << "All corresponding elements should move together." << std::endl;
    
    // Test with data that will actually change
    std::cout << "\n--- Test with data that needs sorting ---" << std::endl;
    test_data[0][2] = {0, 2, 1};     // outward_levels
    test_data[0][5] = {5, 6, 7};     // outward_ranks (should follow levels)
    test_data[0][8] = {10, 20, 30};  // outward_colors (should follow levels)
    
    std::cout << "Before sorting:" << std::endl;
    print_arrays(test_data[0][2], test_data[0][5], test_data[0][8], "outward");
    
    respect_sort_columns(test_data, {2, 5, 8});
    
    std::cout << "After sorting:" << std::endl;
    print_arrays(test_data[0][2], test_data[0][5], test_data[0][8], "outward");
    
    std::cout << "Expected: levels [2, 1, 0], ranks [6, 7, 5], colors [20, 30, 10]" << std::endl;
    std::cout << "(Sorted by levels in descending order, other arrays follow)" << std::endl;
    
    return 0;
}