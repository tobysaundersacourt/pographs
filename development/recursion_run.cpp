#include <iostream>

#include "magnus.hpp"
#include "interface.hpp"

int main () {
    magnus(4);
    std::vector<btree<int, std::vector<std::vector<int>>>> tableaux (magnus_cache.size());
    std::cout << "\033[1;33mBERNOULLI NUMBERS WITH INEQUALITIES\033[0m" << std::endl;
    for (int i = 0; i < magnus_cache.size(); i++) {
        std::cout << "\033[1;34mORDER " << i + 2 << ":\033[0m" << std::endl;
        const auto& pair = magnus_cache[i];
        for (int j = 0; j < pair.first.size(); j++) {
            // for (const auto& element : pair.second[j]) {
            //     std::cout << element << " ";
            // }
            // for (const auto& element : pair.first[j]) {
            //     std::cout << "(" << element[0] << ", " << element[1] << ") ";
            // }
            // std::cout << std::endl;
            add_pograph_reading_inequalities(
                tableaux[i], 
                std::vector<std::vector<int>>{pair.second[j]}, 
                std::vector<std::vector<std::array<int, 2>>>{pair.first[j]}
            );
        }
        tableaux[i].print();
    }
}