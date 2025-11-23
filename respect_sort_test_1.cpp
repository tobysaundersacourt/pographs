#include "respect_sort.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <string>











//The tests need to be updated: Below, the expected results have increasing second vectors for some reason. This is wrong. They should be ascending, given equal first vectors.































template<typename T>
void print_vector(const std::vector<T>& vec, const std::string& name) {
    std::cout << name << ": [";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << vec[i];
    }
    std::cout << "]" << std::endl;
}

void test_basic_sorting() {
    std::vector<int> first = {3, 1, 2, 1};
    std::vector<char> second = {'c', 'a', 'b', 'd'};
    
    std::cout << "\n=== Basic Sorting Test ===" << std::endl;
    std::cout << "Before sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    respect_sort(first, second);
    
    std::cout << "After sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    std::vector<int> expected_first = {3, 2, 1, 1};
    std::vector<char> expected_second = {'c', 'b', 'a', 'd'};
    
    assert(first == expected_first);
    assert(second == expected_second);
    std::cout << "Basic sorting test passed\n";
}

void test_lexicographic_order() {
    std::vector<int> first = {1, 2, 1, 2};
    std::vector<int> second = {2, 1, 1, 2};
    
    std::cout << "\n=== Lexicographic Order Test ===" << std::endl;
    std::cout << "Before sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    respect_sort(first, second);
    
    std::cout << "After sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    std::vector<int> expected_first = {2, 2, 1, 1};
    std::vector<int> expected_second = {1, 2, 1, 2};
    
    assert(first == expected_first);
    assert(second == expected_second);
    std::cout << "Lexicographic order test passed\n";
}

void test_already_sorted() {
    std::vector<int> first = {1, 2, 3, 4};
    std::vector<int> second = {10, 20, 30, 40};
    
    std::cout << "\n=== Already Sorted Test ===" << std::endl;
    std::cout << "Before sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    respect_sort(first, second);
    
    std::cout << "After sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    std::vector<int> expected_first = {4, 3, 2, 1};
    std::vector<int> expected_second = {40, 30, 20, 10};
    
    assert(first == expected_first);
    assert(second == expected_second);
    std::cout << "Already sorted test passed\n";
}

void test_reverse_sorted() {
    std::vector<int> first = {4, 3, 2, 1};
    std::vector<int> second = {40, 30, 20, 10};
    
    std::cout << "\n=== Reverse Sorted Test ===" << std::endl;
    std::cout << "Before sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    respect_sort(first, second);
    
    std::cout << "After sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    std::vector<int> expected_first = {4, 3, 2, 1};
    std::vector<int> expected_second = {40, 30, 20, 10};
    
    assert(first == expected_first);
    assert(second == expected_second);
    std::cout << "Reverse sorted test passed\n";
}

void test_empty_vectors() {
    std::vector<int> first;
    std::vector<int> second;
    
    std::cout << "\n=== Empty Vectors Test ===" << std::endl;
    std::cout << "Before sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    respect_sort(first, second);
    
    std::cout << "After sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    assert(first.empty());
    assert(second.empty());
    std::cout << "Empty vectors test passed\n";
}

void test_single_element() {
    std::vector<int> first = {42};
    std::vector<std::string> second = {"hello"};
    
    std::cout << "\n=== Single Element Test ===" << std::endl;
    std::cout << "Before sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    respect_sort(first, second);
    
    std::cout << "After sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    std::vector<int> expected_first = {42};
    std::vector<std::string> expected_second = {"hello"};
    
    assert(first == expected_first);
    assert(second == expected_second);
    std::cout << "Single element test passed\n";
}

void test_size_mismatch() {
    std::vector<int> first = {1, 2, 3};
    std::vector<int> second = {1, 2};
    
    std::cout << "\n=== Size Mismatch Test ===" << std::endl;
    std::cout << "Input vectors (intentionally mismatched sizes):" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    try {
        respect_sort(first, second);
        assert(false); // Should not reach here
    } catch (const std::invalid_argument&) {
        std::cout << "Expected exception caught for size mismatch" << std::endl;
        std::cout << "Size mismatch test passed\n";
    }
}

void test_different_types() {
    std::vector<double> first = {3.14, 1.41, 2.71};
    std::vector<std::string> second = {"pi", "sqrt2", "e"};
    
    std::cout << "\n=== Different Types Test ===" << std::endl;
    std::cout << "Before sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    respect_sort(first, second);
    
    std::cout << "After sorting:" << std::endl;
    print_vector(first, "first");
    print_vector(second, "second");
    
    std::vector<double> expected_first = {3.14, 2.71, 1.41};
    std::vector<std::string> expected_second = {"pi", "e", "sqrt2"};
    
    assert(first == expected_first);
    assert(second == expected_second);
    std::cout << "Different types test passed\n";
}

int main() {
    test_basic_sorting();
    test_lexicographic_order();
    test_already_sorted();
    test_reverse_sorted();
    test_empty_vectors();
    test_single_element();
    test_size_mismatch();
    test_different_types();
    
    std::cout << "All tests passed!\n";
    return 0;
}
