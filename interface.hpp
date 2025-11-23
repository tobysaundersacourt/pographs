#pragma once
 
#include "abacus_btree_monomials.hpp"
#include "pograph_bicolored.hpp"
#include "linear_extensions.hpp"

#include <tuple>

typedef std::array<int, 3> ce; // colored edge: (from, to, color)
typedef std::vector<ce> bcd; // bicolored directed graph
typedef std::tuple<double, bcd, int> wbcd; // weighted bicolored directed graph
typedef std::vector<wbcd> swbcd; // sum of weighted bicolored directed graphs

template <typename T>
void add_pographs_reading_inequalities(btree<int, T>& b, swbcd graphs, bool discount_by_linear_extensions = false) {
  for (auto graph : graphs) {
      pograph pograph_obj;
      pograph_obj.read_inequalities(std::get<1>(graph));
      T value = static_cast<T>(std::get<0>(graph));
      int degree = std::get<2>(graph);
      std::vector<int> v = pograph_obj.vectorization();
      if (discount_by_linear_extensions) {
          int le_count = count_linear_extensions(pograph_obj);
          if (le_count > 0) {
              value = value / static_cast<T>(le_count);
          }
      }
      b.add(v, value, degree);
  }
}
