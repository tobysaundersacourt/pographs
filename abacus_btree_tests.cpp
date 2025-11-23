#include "abacus_btree_scalars.hpp"

int main () {
    btree<int> btr;
    btr.add({1, 1, 1}, 1);
    btr.add({1, 1}, 1);
    btr.add({2, 3, 4}, 10);
    btr.add({3, 1, 2}, 5);
    btr.add({4, 4, 4}, 20);
    btr.add({2, 2}, 7);
    btr.add({5, 6, 7, 8}, 15);
    btr.add({1, 2, 3, 4, 5}, 25);
    btr.add({3, 3, 3}, 12);
    btr.add({2, 1, 3}, 8);
    btr.add({1}, 2);
    btr.print_nonzero();
    std::cout << "----\n";
    btr.add({1, 1, 1}, 3);
    btr.add({1, 1}, 2);
    btr.add({2, 3, 4}, 5);
    btr.add({3, 1, 2}, 7);
    btr.add({4, 4, 4}, 8);
    btr.add({2, 2}, 4);
    btr.add({5, 6, 7, 8}, 6);
    btr.add({1, 2, 3, 4, 5}, 10);
    btr.add({3, 3, 3}, 2);
    btr.add({2, 1, 3}, 1);
    btr.add({1}, 5);
    btr.print_nonzero();
}
