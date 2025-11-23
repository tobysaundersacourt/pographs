#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <functional>

inline bool greater (std::vector<int>& a, std::vector<int>& b) {
    int n = a.size();
    int m = b.size();
    int min = std::min(n, m);
    for (int i = 0; i < min; i++) {
        if (a[i] < b[i]) {
            return false;
        }
        else if (a[i] > b[i]) {
            return true;
        }
    }
    if (n > m) {
        return true;
    }
    return false;
}

std::function<bool(std::vector<int>&, std::vector<int>&)> greater_obj = greater;

// meaning of "conclusive": returns 1 if a > b, 2 if b > a, and 0 if a == b
inline int conclusive_greater (std::vector<int>& a, std::vector<int>& b) {
    int n = a.size();
    int m = b.size();
    int min = std::min(n, m);
    for (int i = 0; i < min; i++) {
        if (a[i] < b[i]) {
            return 2;
        }
        else if (a[i] > b[i]) {
            return 1;
        }
    }
    if (n > m) {
        return 1;
    }
    else if (m > n) {
        return 2;
    }
    return 0;
}

inline bool compare_vertex_data (std::array<std::vector<int>, 6>& a, std::array<std::vector<int>, 6>& b) {
    int comp = conclusive_greater(a[3], b[3]);
    if (comp == 0) {
        comp = conclusive_greater(a[0], b[0]);
        if (comp == 0) {
            comp = conclusive_greater(a[4], b[4]);
            if (comp == 0) {
                comp = conclusive_greater(a[1], b[1]);
                if (comp == 0) {
                    comp = conclusive_greater(a[5], b[5]);
                    if (comp == 0) {
                        return greater(a[2], b[2]);
                    }
                    return comp == 1;
                }
                return comp == 1;
            }
            return comp == 1;
        }
        return comp == 1;
    }
    return comp == 1;
}

std::function<bool(std::array<std::vector<int>, 6>&, std::array<std::vector<int>, 6>&)> compare_vertex_data_obj = static_cast<bool(*)(std::array<std::vector<int>, 6>&, std::array<std::vector<int>, 6>&)>(compare_vertex_data);

inline int conclusive_compare_vertex_data (std::array<std::vector<int>, 6>& a, std::array<std::vector<int>, 6>& b) {
    std::cout << "      Comparing vertex data arrays (6-element):" << std::endl;
    
    int comp = conclusive_greater(a[3], b[3]);
    std::cout << "        Ranks[3] comparison: A=[";
    for (int i = 0; i < a[3].size(); i++) std::cout << a[3][i] << (i < a[3].size()-1 ? ", " : "");
    std::cout << "] vs B=[";
    for (int i = 0; i < b[3].size(); i++) std::cout << b[3][i] << (i < b[3].size()-1 ? ", " : "");
    std::cout << "] -> result: " << comp << std::endl;
    if (comp == 0) {
        comp = conclusive_greater(a[0], b[0]);
        std::cout << "        Levels[0] comparison: A=[";
        for (int i = 0; i < a[0].size(); i++) std::cout << a[0][i] << (i < a[0].size()-1 ? ", " : "");
        std::cout << "] vs B=[";
        for (int i = 0; i < b[0].size(); i++) std::cout << b[0][i] << (i < b[0].size()-1 ? ", " : "");
        std::cout << "] -> result: " << comp << std::endl;
        if (comp == 0) {
            comp = conclusive_greater(a[4], b[4]);
            std::cout << "        Internal ranks[4] comparison: A=[";
            for (int i = 0; i < a[4].size(); i++) std::cout << a[4][i] << (i < a[4].size()-1 ? ", " : "");
            std::cout << "] vs B=[";
            for (int i = 0; i < b[4].size(); i++) std::cout << b[4][i] << (i < b[4].size()-1 ? ", " : "");
            std::cout << "] -> result: " << comp << std::endl;
            if (comp == 0) {
                comp = conclusive_greater(a[1], b[1]);
                std::cout << "        Internal levels[1] comparison: A=[";
                for (int i = 0; i < a[1].size(); i++) std::cout << a[1][i] << (i < a[1].size()-1 ? ", " : "");
                std::cout << "] vs B=[";
                for (int i = 0; i < b[1].size(); i++) std::cout << b[1][i] << (i < b[1].size()-1 ? ", " : "");
                std::cout << "] -> result: " << comp << std::endl;
                if (comp == 0) {
                    comp = conclusive_greater(a[5], b[5]);
                    std::cout << "        Outward ranks[5] comparison: A=[";
                    for (int i = 0; i < a[5].size(); i++) std::cout << a[5][i] << (i < a[5].size()-1 ? ", " : "");
                    std::cout << "] vs B=[";
                    for (int i = 0; i < b[5].size(); i++) std::cout << b[5][i] << (i < b[5].size()-1 ? ", " : "");
                    std::cout << "] -> result: " << comp << std::endl;
                    if (comp == 0) {
                        comp = conclusive_greater(a[2], b[2]);
                        std::cout << "        Outward levels[2] comparison: A=[";
                        for (int i = 0; i < a[2].size(); i++) std::cout << a[2][i] << (i < a[2].size()-1 ? ", " : "");
                        std::cout << "] vs B=[";
                        for (int i = 0; i < b[2].size(); i++) std::cout << b[2][i] << (i < b[2].size()-1 ? ", " : "");
                        std::cout << "] -> result: " << comp << std::endl;
                        return comp;
                    }
                    return comp;
                }
                return comp;
            }
            return comp;
        }
        return comp;
    }
    return comp;
}

inline bool compare_vertex_data (std::array<std::vector<int>, 9>& a, std::array<std::vector<int>, 9>& b) {
     int comp = conclusive_greater(a[3], b[3]);
     if (comp == 0) {
         comp = conclusive_greater(a[0], b[0]);
         if (comp == 0) {
             comp = conclusive_greater(a[6], b[6]);
                if (comp == 0) {
                    comp = conclusive_greater(a[4], b[4]);
                    if (comp == 0) {
                        comp = conclusive_greater(a[1], b[1]);
                        if (comp == 0) {
                            comp = conclusive_greater(a[7], b[7]);
                            if (comp == 0) {
                                comp = conclusive_greater(a[5], b[5]);
                                if (comp == 0) {
                                    comp = conclusive_greater(a[2], b[2]);
                                    if (comp == 0) {
                                        return greater(a[8], b[8]);
                                    }
                                    return comp == 1;
                                }
                                return comp == 1;
                            }
                            return comp == 1;
                        }
                        return comp == 1;
                    }
                    return comp == 1;
                }
                return comp == 1;
         }
         return comp == 1;
     }
     return comp == 1;
 }
 
 std::function<bool(std::array<std::vector<int>, 9>&, std::array<std::vector<int>, 9>&)> compare_vertex_data_obj_9 = static_cast<bool(*)(std::array<std::vector<int>, 9>&, std::array<std::vector<int>, 9>&)>(compare_vertex_data);

inline int conclusive_compare_vertex_data (std::array<std::vector<int>, 9>& a, std::array<std::vector<int>, 9>& b) {
  // std::cout << "      Comparing vertex data arrays (9-element):" << std::endl;
  
  int comp = conclusive_greater(a[3], b[3]);
  // std::cout << "        Ranks[3] comparison: A=[";
  // for (int i = 0; i < a[3].size(); i++) std::cout << a[3][i] << (i < a[3].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[3].size(); i++) std::cout << b[3][i] << (i < b[3].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
      return comp;
  }
  
  comp = conclusive_greater(a[0], b[0]);
  // std::cout << "        Levels[0] comparison: A=[";
  // for (int i = 0; i < a[0].size(); i++) std::cout << a[0][i] << (i < a[0].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[0].size(); i++) std::cout << b[0][i] << (i < b[0].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
     return comp;
  }
  
  comp = conclusive_greater(a[6], b[6]);
  // std::cout << "        Colors[6] comparison: A=[";
  // for (int i = 0; i < a[6].size(); i++) std::cout << a[6][i] << (i < a[6].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[6].size(); i++) std::cout << b[6][i] << (i < b[6].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
      return comp;
  }
  
  comp = conclusive_greater(a[4], b[4]);
  // std::cout << "        Internal ranks[4] comparison: A=[";
  // for (int i = 0; i < a[4].size(); i++) std::cout << a[4][i] << (i < a[4].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[4].size(); i++) std::cout << b[4][i] << (i < b[4].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
      return comp;
  }
  
  comp = conclusive_greater(a[1], b[1]);
  // std::cout << "        Internal levels[1] comparison: A=[";
  // for (int i = 0; i < a[1].size(); i++) std::cout << a[1][i] << (i < a[1].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[1].size(); i++) std::cout << b[1][i] << (i < b[1].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
      return comp;
  }
  
  comp = conclusive_greater(a[7], b[7]);
  // std::cout << "        Internal colors[7] comparison: A=[";
  // for (int i = 0; i < a[7].size(); i++) std::cout << a[7][i] << (i < a[7].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[7].size(); i++) std::cout << b[7][i] << (i < b[7].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
      return comp;
  }
  
  comp = conclusive_greater(a[5], b[5]);
  // std::cout << "        Outward ranks[5] comparison: A=[";
  // for (int i = 0; i < a[5].size(); i++) std::cout << a[5][i] << (i < a[5].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[5].size(); i++) std::cout << b[5][i] << (i < b[5].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
      return comp;
  }
  
  comp = conclusive_greater(a[2], b[2]);
  // std::cout << "        Outward levels[2] comparison: A=[";
  // for (int i = 0; i < a[2].size(); i++) std::cout << a[2][i] << (i < a[2].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[2].size(); i++) std::cout << b[2][i] << (i < b[2].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  if (comp != 0) {
      return comp;
  }
  
  comp = conclusive_greater(a[8], b[8]);
  // std::cout << "        Outward colors[8] comparison: A=[";
  // for (int i = 0; i < a[8].size(); i++) std::cout << a[8][i] << (i < a[8].size()-1 ? ", " : "");
  // std::cout << "] vs B=[";
  // for (int i = 0; i < b[8].size(); i++) std::cout << b[8][i] << (i < b[8].size()-1 ? ", " : "");
  // std::cout << "] -> result: " << comp << std::endl;
  return comp;
}

inline bool layer_comparison (std::vector<std::array<std::vector<int>, 6>>& a, std::vector<std::array<std::vector<int>, 6>>& b) {
    int n = a.size();
    int m = b.size();
    int min = std::min(n, m);
    for (int i = 0; i < min; i++) {
        int comp = conclusive_compare_vertex_data(a[i], b[i]);
        if (comp != 0) {
            return comp == 1;
        }
    }
    if (n > m) {
        return true;
    }
    return false;
} 

inline bool layer_comparison (std::vector<std::array<std::vector<int>, 9>>& a, std::vector<std::array<std::vector<int>, 9>>& b) {
    int n = a.size();
    int m = b.size();
    int min = std::min(n, m);
    for (int i = 0; i < min; i++) {
        int comp = conclusive_compare_vertex_data(a[i], b[i]);
        if (comp != 0) {
            return comp == 1;
        }
    }
    if (n > m) {
        return true;
    }
    return false;
}

std::function<bool(std::vector<std::array<std::vector<int>, 6>>&, std::vector<std::array<std::vector<int>, 6>>&)> layer_comparison_obj = static_cast<bool(*)(std::vector<std::array<std::vector<int>, 6>>&, std::vector<std::array<std::vector<int>, 6>>&)>(layer_comparison);

inline int conclusive_layer_comparison (std::vector<std::array<std::vector<int>, 6>>& a, std::vector<std::array<std::vector<int>, 6>>& b) {

    int n = a.size();
    int m = b.size();
    int min = std::min(n, m);
    std::cout << "    Layer comparison (6-element): A has " << n << " nodes, B has " << m << " nodes" << std::endl;
    for (int i = 0; i < min; i++) {
        std::cout << "    Comparing node pair " << i << ":" << std::endl;
        int comp = conclusive_compare_vertex_data(a[i], b[i]);
        std::cout << "    Node pair " << i << " result: " << comp << std::endl;
        if (comp != 0) {
            std::cout << "    Layer comparison decided by node pair " << i << ": " << (comp == 1 ? "A > B" : "B > A") << std::endl;
            return comp;
        }
    }
    if (n > m) {
        std::cout << "    Layer comparison decided by count: A has more nodes (" << n << " vs " << m << ")" << std::endl;
        return 1;
    }
    else if (m > n) {
        std::cout << "    Layer comparison decided by count: B has more nodes (" << m << " vs " << n << ")" << std::endl;
        return 2;
    }
    std::cout << "    Layer comparison: all nodes equal and same count" << std::endl;
    return 0;
} 

std::function<bool(std::vector<std::array<std::vector<int>, 9>>&, std::vector<std::array<std::vector<int>, 9>>&)> layer_comparison_obj_9 = static_cast<bool(*)(std::vector<std::array<std::vector<int>, 9>>&, std::vector<std::array<std::vector<int>, 9>>&)>(layer_comparison);
 
inline int conclusive_layer_comparison (std::vector<std::array<std::vector<int>, 9>>& a, std::vector<std::array<std::vector<int>, 9>>& b) {
 int n = a.size();
 int m = b.size();
 int min = std::min(n, m);
 // std::cout << "    Layer comparison: A has " << n << " nodes, B has " << m << " nodes" << std::endl;
 for (int i = 0; i < min; i++) {
     // std::cout << "    Comparing node pair " << i << ":" << std::endl;
     int comp = conclusive_compare_vertex_data(a[i], b[i]);
     // std::cout << "    Node pair " << i << " result: " << comp << std::endl;
     if (comp != 0) {
         // std::cout << "    Layer comparison decided by node pair " << i << ": " << (comp == 1 ? "A > B" : "B > A") << std::endl;
         return comp;
     }
 }
 if (n > m) {
     // std::cout << "    Layer comparison decided by count: A has more nodes (" << n << " vs " << m << ")" << std::endl;
     return 1;
 }
 else if (m > n) {
     // std::cout << "    Layer comparison decided by count: B has more nodes (" << m << " vs " << n << ")" << std::endl;
     return 2;
 }
 // std::cout << "    Layer comparison: all nodes equal and same count" << std::endl;
 return 0;
}

void sort_descending_int_vector(std::vector<int>& v) {
    std::sort(v.begin(), v.end(), std::greater<int>());
}

void sort_descending_int_vector_vector(std::vector<std::vector<int>>& v) {
    std::sort(v.begin(), v.end(), greater_obj);
}

void sort_descending_vertex_data(std::vector<std::array<std::vector<int>, 6>>& v) {
    std::sort(v.begin(), v.end(), compare_vertex_data_obj);
}

void sort_descending_layers (std::vector<std::vector<std::array<std::vector<int>, 6>>>& v) {
    std::sort(v.begin(), v.end(), layer_comparison_obj);
}

void sort_descending_vertex_data(std::vector<std::array<std::vector<int>, 9>>& v) {
 std::sort(v.begin(), v.end(), compare_vertex_data_obj_9);
}

void sort_descending_layers (std::vector<std::vector<std::array<std::vector<int>, 9>>>& v) {
 std::sort(v.begin(), v.end(), layer_comparison_obj_9);
}

