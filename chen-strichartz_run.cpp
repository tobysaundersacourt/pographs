#include <iostream>
#include <cstdlib>
 
#include "chen-strichartz.hpp"
#include "interface.hpp"
#include "graph_visualizer.hpp"
 
int main (int argc, char* argv[]) {
     // Parse command line arguments
     int degree = -1;
     int loop_order = 1;
     bool bicolored = true;
     bool output_mathematica = false;
     std::string mathematica_file = "";
     
     for (int i = 1; i < argc; i++) {
         std::string arg = argv[i];
         
         if (arg == "--degree") {
             if (i + 1 < argc) {
                 degree = std::atoi(argv[++i]);
             } else {
                 std::cerr << "Error: --degree requires a value" << std::endl;
                 return 1;
             }
         }
         else if (arg == "--loop-order") {
             if (i + 1 < argc) {
                 loop_order = std::atoi(argv[++i]);
             } else {
                 std::cerr << "Error: --loop-order requires a value" << std::endl;
                 return 1;
             }
         }
         else if (arg == "--bicolored") {
             if (i + 1 < argc) {
                 std::string bicolored_value = argv[++i];
                 if (bicolored_value == "true" || bicolored_value == "1") {
                     bicolored = true;
                 } else if (bicolored_value == "false" || bicolored_value == "0") {
                     bicolored = false;
                 } else {
                     std::cerr << "Error: --bicolored requires 'true', 'false', '1', or '0'" << std::endl;
                     return 1;
                 }
             } else {
                 std::cerr << "Error: --bicolored requires a value" << std::endl;
                 return 1;
             }
         }
         else if (arg == "--mathematica") {
             output_mathematica = true;
             if (i + 1 < argc && argv[i + 1][0] != '-') {
                 mathematica_file = argv[++i];
             } else {
                 mathematica_file = "output.m";
             }
         }
         else {
             std::cerr << "Unknown flag: " << arg << std::endl;
             return 1;
         }
     }
     
     if (degree == -1) {
         std::cerr << "Usage: " << argv[0] << " --degree <degree> [--loop-order <order>] [--bicolored <true|false>] [--mathematica [filename]]" << std::endl;
         return 1;
     }
     std::cout << "\n";
     auto graphs = cs(degree, loop_order, bicolored);
     btree<int, double> table_undiscounted;
     std::cout << "\033[1;33mCoefficients with Graph Codes (Undiscounted)\033[0m" << std::endl;
     add_pographs_reading_inequalities(
          table_undiscounted,
          graphs,
          false 
     );
     table_undiscounted.print_nonzero(1e-10);
     std::cout << "\n";
     btree<int, double> table;
     std::cout << "\033[1;33mCoefficients with Graph Codes (Discounted)\033[0m" << std::endl;
     add_pographs_reading_inequalities(
         table,
         graphs,
         true
     );
     table.print_nonzero(1e-10);
     std::cout << "\n";
     // Get weighted paths and visualize graphs
     auto weighted_paths = table.get_weighted_paths();
     
     // Visualize bicolored graphs using BCD format (always colored format)
     if (output_mathematica) {
         visualizeBicoloredWeightedGraphs(weighted_paths, true, mathematica_file, true);
     } else {
         visualizeBicoloredWeightedGraphs(weighted_paths, false, "", true);
     }
     std::cout << "\n";
 }
