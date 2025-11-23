#include "graph_visualizer.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <algorithm>

void Graph::addEdge(int from, int to) {
    edges.push_back({from, to});
}

std::string Graph::toString() const {
    if (edges.empty()) {
        return "∅";  // Empty graph symbol
    }
    
    std::string result = "{";
    for (size_t i = 0; i < edges.size(); ++i) {
        if (i > 0) result += ", ";
        result += "(" + std::to_string(edges[i].first) + "→" + std::to_string(edges[i].second) + ")";
    }
    result += "}";
    return result;
}

std::string Graph::toMathematica() const {
    if (edges.empty()) {
        return "Graph[{}, {}]";
    }
    
    // Collect all vertices
    std::set<int> vertices;
    for (const auto& edge : edges) {
        vertices.insert(edge.first);
        vertices.insert(edge.second);
    }
    
    // Build vertex list
    std::string vertexList = "{";
    bool first = true;
    for (int v : vertices) {
        if (!first) vertexList += ", ";
        vertexList += std::to_string(v);
        first = false;
    }
    vertexList += "}";
    
    // Build edge list
    std::string edgeList = "{";
    first = true;
    for (const auto& edge : edges) {
        if (!first) edgeList += ", ";
        edgeList += "DirectedEdge[" + std::to_string(edge.first) + ", " + std::to_string(edge.second) + "]";
        first = false;
    }
    edgeList += "}";
    
    return "Graph[" + vertexList + ", " + edgeList + "]";
}

bool Graph::isEmpty() const {
    return edges.empty();
}

Graph parseGraphCode(const std::vector<int>& code) {
    Graph graph;
    
    if (code.empty()) {
        return graph;
    }
    
    // Reverse the code since btree outputs paths in reverse order
    std::vector<int> reversedCode(code.rbegin(), code.rend());
    
    // Find the first 0 (start of actual graph data)
    size_t start = 0;
    while (start < reversedCode.size() && reversedCode[start] != 0) {
        start++;
    }
    
    if (start >= reversedCode.size()) {
        return graph; // No valid graph data found
    }
    
    // Track which values we've seen (are old nodes)
    std::set<int> seenValues;
    int mostRecentNewNode = -1;
    
    for (size_t i = start; i < reversedCode.size(); ++i) {
        int value = reversedCode[i];
        
        if (seenValues.find(value) == seenValues.end()) {
            // New node
            seenValues.insert(value);
            mostRecentNewNode = value;
        } else {
            // Old node - create edge from most recent new node to this old node
            if (mostRecentNewNode != -1) {
                graph.addEdge(mostRecentNewNode, value);
            }
        }
    }
    
    return graph;
}

void displayMathematicalExpression(const std::vector<std::pair<double, std::vector<int>>>& weighted_paths) {
    std::cout << "\033[1;33mMathematical Expression\033[0m" << std::endl;
    
    if (weighted_paths.empty()) {
        std::cout << "0" << std::endl;
        return;
    }
    
    bool isFirst = true;
    for (const auto& entry : weighted_paths) {
        double coefficient = entry.first;
        
        if (std::abs(coefficient) < 1e-10) {
            continue; // Skip near-zero coefficients
        }
        
        Graph graph = parseGraphCode(entry.second);
        
        // Handle sign and spacing
        if (!isFirst) {
            if (coefficient > 0) {
                std::cout << " + ";
            } else {
                std::cout << " - ";
                coefficient = -coefficient;
            }
        } else {
            if (coefficient < 0) {
                std::cout << "-";
                coefficient = -coefficient;
            }
            isFirst = false;
        }
        
        // Display coefficient if it's not 1 or if graph is empty
        if (std::abs(coefficient - 1.0) > 1e-10 || graph.isEmpty()) {
            std::cout << std::fixed << std::setprecision(3) << coefficient;
        }
        
        // Display graph
        std::cout << "(" << graph.toString() << ")";
    }
}

std::string generateMathematicaCode(const std::vector<std::pair<double, std::vector<int>>>& weighted_paths) {
    if (weighted_paths.empty()) {
        return "0";
    }
    
    std::ostringstream mathematica;
    mathematica << "(* Weighted sum of graphs *)\n";
    mathematica << std::fixed << std::setprecision(6);
    
    bool isFirst = true;
    for (const auto& entry : weighted_paths) {
        double coefficient = entry.first;
        
        if (std::abs(coefficient) < 1e-10) {
            continue; // Skip near-zero coefficients
        }
        
        Graph graph = parseGraphCode(entry.second);
        
        // Handle sign and spacing
        if (!isFirst) {
            if (coefficient > 0) {
                mathematica << " + ";
            } else {
                mathematica << " - ";
                coefficient = -coefficient;
            }
        } else {
            if (coefficient < 0) {
                mathematica << "-";
                coefficient = -coefficient;
            }
            isFirst = false;
        }
        
        // Add coefficient and graph
        if (std::abs(coefficient - 1.0) > 1e-10 || graph.isEmpty()) {
            mathematica << coefficient << "*";
        }
        
        mathematica << graph.toMathematica();
    }
    
    return mathematica.str();
}

void visualizeWeightedGraphs(const std::vector<std::pair<double, std::vector<int>>>& weighted_paths, 
                            bool output_mathematica, 
                            const std::string& mathematica_filename) {
    
    // Always display terminal output
    displayMathematicalExpression(weighted_paths);
   
    std::cout << std::endl;
    // Optionally generate Mathematica output
    if (output_mathematica) {
        std::string mathematicaCode = generateMathematicaCode(weighted_paths);
        
        // Write to file
        std::ofstream file(mathematica_filename);
        if (file.is_open()) {
            file << mathematicaCode << std::endl;
            file.close();
            std::cout << "Mathematica code written to: " << mathematica_filename << std::endl;
        } else {
            std::cerr << "Error: Could not write to file " << mathematica_filename << std::endl;
        }
    }
}

// BicoloredGraph implementation
void BicoloredGraph::addEdge(int from, int to, int color) {
    edges.push_back({from, to, color});
}

std::string BicoloredGraph::toString() const {
    if (edges.empty()) {
        return "∅";  // Empty graph symbol
    }
    
    std::string result = "{";
    for (size_t i = 0; i < edges.size(); ++i) {
        if (i > 0) result += ", ";
        std::string color = (std::get<2>(edges[i]) == 0) ? "red" : "blue";
        result += "(" + std::to_string(std::get<0>(edges[i])) + "→" + 
                  std::to_string(std::get<1>(edges[i])) + "," + color + ")";
    }
    result += "}";
    return result;
}

std::string BicoloredGraph::toMathematica() const {
    if (edges.empty()) {
        return "Graph[{}, {}]";
    }
    
    // Collect all vertices
    std::set<int> vertices;
    for (const auto& edge : edges) {
        vertices.insert(std::get<0>(edge));
        vertices.insert(std::get<1>(edge));
    }
    
    // Build vertex list
    std::string vertexList = "{";
    bool first = true;
    for (int v : vertices) {
        if (!first) vertexList += ", ";
        vertexList += std::to_string(v);
        first = false;
    }
    vertexList += "}";
    
    // Build edge list with colors
    std::string edgeList = "{";
    first = true;
    for (const auto& edge : edges) {
        if (!first) edgeList += ", ";
        int from = std::get<0>(edge);
        int to = std::get<1>(edge);
        int color = std::get<2>(edge);
        std::string colorStyle = (color == 0) ? "Red" : "Blue";
        edgeList += "Style[DirectedEdge[" + std::to_string(from) + ", " + 
                   std::to_string(to) + "], " + colorStyle + "]";
        first = false;
    }
    edgeList += "}";
    
    return "Graph[" + vertexList + ", " + edgeList + ", VertexLabels -> \"Name\"]";
}

bool BicoloredGraph::isEmpty() const {
    return edges.empty();
}

BicoloredGraph parseBicoloredGraphCode(const std::vector<int>& code) {
    BicoloredGraph graph;
    
    if (code.empty()) {
        return graph;
    }
    
    // Reverse the code since btree outputs paths in reverse order
    std::vector<int> reversedCode(code.rbegin(), code.rend());
    
    // Find the first 0 (start of actual graph data)
    size_t start = 0;
    while (start < reversedCode.size() && reversedCode[start] != 0) {
        start++;
    }
    
    if (start >= reversedCode.size()) {
        return graph; // No valid graph data found
    }
    
    // Track which values we've seen (are old nodes)
    std::set<int> seenValues;
    int currentNode = -1; // The most recent new node we're considering
    
    for (size_t i = start; i < reversedCode.size(); ++i) {
        int value = reversedCode[i];
        
        if (seenValues.find(value) == seenValues.end()) {
            // New node - assign it as the current node under consideration
            seenValues.insert(value);
            currentNode = value;
        } else {
            // Old node - enter node-making state
            // Next value should be the edge color
            if (i + 1 < reversedCode.size() && currentNode != -1) {
                int color = reversedCode[i + 1];
                graph.addEdge(currentNode, value, color); // Edge from current to old
                ++i; // Skip the color value in next iteration
                // Return to empty state (currentNode stays the same until we see a new node)
            }
        }
    }
    
    return graph;
}

std::string formatPolynomial(const std::vector<double>& coeffs) {
    if (coeffs.empty()) {
        return "0";
    }
    
    std::ostringstream poly;
    bool isFirst = true;
    
    for (size_t i = 0; i < coeffs.size(); ++i) {
        double coeff = coeffs[i];
        
        if (std::abs(coeff) < 1e-10) {
            continue; // Skip near-zero coefficients
        }
        
        // Handle sign and spacing
        if (!isFirst) {
            if (coeff > 0) {
                poly << " + ";
            } else {
                poly << " - ";
                coeff = -coeff;
            }
        } else {
            if (coeff < 0) {
                poly << "-";
                coeff = -coeff;
            }
            isFirst = false;
        }
        
        // Handle coefficient
        if (i == 0) {
            // Constant term
            poly << std::fixed << std::setprecision(6) << coeff;
        } else if (std::abs(coeff - 1.0) < 1e-10) {
            // Coefficient is 1, just show variable
            if (i == 1) {
                poly << "h";
            } else {
                poly << "h^" << i;
            }
        } else {
            // General case
            poly << std::fixed << std::setprecision(6) << coeff << "*";
            if (i == 1) {
                poly << "h";
            } else {
                poly << "h^" << i;
            }
        }
    }
    
    return isFirst ? "0" : poly.str();
}

void displayBicoloredMathematicalExpression(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths) {
    std::cout << "\033[1;33mBicolored Mathematical Expression\033[0m" << std::endl;
    
    if (weighted_paths.empty()) {
        std::cout << "0" << std::endl;
        return;
    }
    
    bool isFirst = true;
    for (const auto& entry : weighted_paths) {
        const std::vector<double>& polynomial = entry.first;
        
        // Check if polynomial is effectively zero
        bool isZero = true;
        for (double coeff : polynomial) {
            if (std::abs(coeff) >= 1e-10) {
                isZero = false;
                break;
            }
        }
        if (isZero) continue;
        
        BicoloredGraph graph = parseBicoloredGraphCode(entry.second);
        
        // Format the polynomial coefficient
        std::string polyStr = formatPolynomial(polynomial);
        
        if (!isFirst) {
            std::cout << " + ";
        }
        isFirst = false;
        
        // Display polynomial and graph
        std::cout << "(" << polyStr << ")(" << graph.toString() << ")";
    }
}

std::string generateBicoloredMathematicaCode(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths) {
    if (weighted_paths.empty()) {
        return "0";
    }
    
    std::ostringstream mathematica;
    mathematica << "(* Weighted sum of bicolored graphs with polynomial coefficients *)\n";
    
    bool isFirst = true;
    for (const auto& entry : weighted_paths) {
        const std::vector<double>& polynomial = entry.first;
        
        // Check if polynomial is effectively zero
        bool isZero = true;
        for (double coeff : polynomial) {
            if (std::abs(coeff) >= 1e-10) {
                isZero = false;
                break;
            }
        }
        if (isZero) continue;
        
        BicoloredGraph graph = parseBicoloredGraphCode(entry.second);
        
        if (!isFirst) {
            mathematica << " + ";
        }
        isFirst = false;
        
        // Format polynomial and graph
        std::string polyStr = formatPolynomial(polynomial);
        mathematica << "(" << polyStr << ")*" << graph.toMathematica();
    }
    
    return mathematica.str();
}

std::string generateBicoloredMathematicaBCDFormat(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths) {
    std::ostringstream result;
    result << "myBCDgraphs = {\n";
    
    bool first = true;
    for (const auto& entry : weighted_paths) {
        const std::vector<double>& polynomial = entry.first;
        
        // Check if polynomial is effectively zero
        bool isZero = true;
        for (double coeff : polynomial) {
            if (std::abs(coeff) >= 1e-10) {
                isZero = false;
                break;
            }
        }
        if (isZero) continue;
        
        if (!first) result << ",\n";
        first = false;
        
        BicoloredGraph graph = parseBicoloredGraphCode(entry.second);
        
        // Format polynomial coefficient
        std::string polyStr = formatPolynomial(polynomial);
        result << "  {" << polyStr << ", {";
        
        bool firstEdge = true;
        for (const auto& edge : graph.edges) {
            if (!firstEdge) result << ", ";
            firstEdge = false;
            
            int from = std::get<0>(edge);
            int to = std::get<1>(edge);
            int color = std::get<2>(edge);
            std::string colorName = (color == 0) ? "Cm" : "Cp";
            result << "{" << from << " -> " << to << ", " << colorName << "}";
        }
        
        result << "}}";
    }
    
    result << "\n};\n\nviews[myBCDgraphs]";
    return result.str();
}

void visualizeBicoloredWeightedGraphs(const std::vector<std::pair<std::vector<double>, std::vector<int>>>& weighted_paths, 
                                    bool output_mathematica, 
                                    const std::string& mathematica_filename,
                                    bool use_bcd_format) {
    
    // Always display terminal output
    displayBicoloredMathematicalExpression(weighted_paths);
   
    std::cout << std::endl;
    // Optionally generate Mathematica output
    if (output_mathematica) {
        std::string mathematicaCode;
        if (use_bcd_format) {
            mathematicaCode = generateBicoloredMathematicaBCDFormat(weighted_paths);
        } else {
            mathematicaCode = generateBicoloredMathematicaCode(weighted_paths);
        }
        
        // Write to file
        std::ofstream file(mathematica_filename);
        if (file.is_open()) {
            file << mathematicaCode << std::endl;
            file.close();
            std::cout << "Mathematica code written to: " << mathematica_filename << std::endl;
        } else {
            std::cerr << "Error: Could not write to file " << mathematica_filename << std::endl;
        }
    }
}
