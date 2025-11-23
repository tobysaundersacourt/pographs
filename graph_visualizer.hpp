#pragma once

#include <vector>
#include <string>
#include <utility>

struct Graph {
    std::vector<std::pair<int, int>> edges;
    
    void addEdge(int from, int to);
    std::string toString() const;
    std::string toMathematica() const;
    bool isEmpty() const;
};

struct BicoloredGraph {
    std::vector<std::tuple<int, int, int>> edges; // from, to, color (0=red, 1=blue)
    
    void addEdge(int from, int to, int color);
    std::string toString() const;
    std::string toMathematica() const;
    bool isEmpty() const;
};

// Parse graph code according to the specification
Graph parseGraphCode(const std::vector<int>& code);

// Parse bicolored graph code according to the new specification
BicoloredGraph parseBicoloredGraphCode(const std::vector<int>& code);

// Display mathematical expression in terminal
void displayMathematicalExpression(const std::vector<std::pair<double, std::vector<int>>>& weighted_paths);

// Generate Mathematica code for the weighted sum
std::string generateMathematicaCode(const std::vector<std::pair<double, std::vector<int>>>& weighted_paths);

// Main visualization function that handles both terminal and Mathematica output
void visualizeWeightedGraphs(const std::vector<std::pair<double, std::vector<int>>>& weighted_paths, 
                            bool output_mathematica = false, 
                            const std::string& mathematica_filename = "");

// Bicolored graph visualization functions (with polynomial coefficients)
void displayBicoloredMathematicalExpression(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths);
std::string generateBicoloredMathematicaCode(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths);
std::string generateBicoloredMathematicaBCDFormat(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths);
void visualizeBicoloredWeightedGraphs(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths, 
                                    bool output_mathematica = false, 
                                    const std::string& mathematica_filename = "",
                                    bool use_bcd_format = true);

// Helper function to format polynomial
std::string formatPolynomial(const std::vector<double>& coeffs);