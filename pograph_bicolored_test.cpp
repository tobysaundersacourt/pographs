#include "pograph_bicolored.hpp"
#include <vector>

int main () {
    pograph g;
    // 0 1 0 0 2 0 0 3 1 0 
    g.read_inequalities({{0,1,0},{0,2,0},{1,3,0}});
        std::vector<int> v = g.vectorization();
    //print vector
    for (int i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    pograph g2;
    g2.read_inequalities({{1,0,0},{2,0,0},{3,1,0}});
    std::vector<int> v2 = g2.vectorization();
    //print vector
    for (int i : v2) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    return 0;
}
