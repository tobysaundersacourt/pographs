#include <iostream>
#include <vector>
#include <set>
#include <sstream>

struct ColoredEdge {
    int from;
    int to; 
    int color; // 0 = Cm (red), 1 = Cp (blue)
};

std::vector<ColoredEdge> convertVectorizationToEdges(const std::vector<int>& code) {
    std::vector<ColoredEdge> edges;
    
    if (code.empty()) {
        return edges;
    }
    
    // Find the first 0 (start of actual graph data)
    size_t start = 0;
    while (start < code.size() && code[start] != 0) {
        start++;
    }
    
    if (start >= code.size()) {
        return edges; // No valid graph data found
    }
    
    // Track which values we've seen (are old nodes)
    std::set<int> seenValues;
    int mostRecentNewNode = -1;
    
    for (size_t i = start; i < code.size(); ++i) {
        int value = code[i];
        
        if (seenValues.find(value) == seenValues.end()) {
            // New node
            seenValues.insert(value);
            mostRecentNewNode = value;
        } else {
            // Old node - next value should be the edge color
            if (i + 1 < code.size() && mostRecentNewNode != -1) {
                int color = code[i + 1];
                edges.push_back({mostRecentNewNode, value, color});
                ++i; // Skip the color value in next iteration
            }
        }
    }
    
    return edges;
}

std::string generateMathematicaBCDFormat(const std::vector<std::pair<double, std::vector<int>>>& weighted_paths) {
    std::ostringstream result;
    result << "myBCDgraphs = {\n";
    
    bool first = true;
    for (const auto& entry : weighted_paths) {
        if (!first) result << ",\n";
        first = false;
        
        double coefficient = entry.first;
        std::vector<ColoredEdge> edges = convertVectorizationToEdges(entry.second);
        
        result << "  {" << coefficient << ", {";
        
        bool firstEdge = true;
        for (const auto& edge : edges) {
            if (!firstEdge) result << ", ";
            firstEdge = false;
            
            std::string colorName = (edge.color == 0) ? "Cm" : "Cp";
            result << "{" << edge.from << " -> " << edge.to << ", " << colorName << "}";
        }
        
        result << "}}";
    }
    
    result << "\n};\n\nviews[myBCDgraphs]";
    return result.str();
}

int main() {
    // Test with our previous examples
    std::vector<std::pair<double, std::vector<int>>> test_data = {
        {0.5, {1, 1, 1, 0, 1, 0, 0, 2, 1, 1}},
        {-0.3, {1, 2, 0, 1, 0, 0, 2, 0, 1}},
        {0.2, {1, 2, 1, 0, 1, 0, 0, 2, 0, 1, 3, 2, 1, 1, 0}}
    };
    
    std::cout << "Converting vectorizations to AutoWightman BCD format:\n\n";
    
    // Show individual conversions for debugging
    for (size_t i = 0; i < test_data.size(); i++) {
        std::cout << "Example " << (i+1) << " vectorization: ";
        for (int x : test_data[i].second) std::cout << x << " ";
        std::cout << "\n";
        
        auto edges = convertVectorizationToEdges(test_data[i].second);
        std::cout << "Converted edges: ";
        for (const auto& edge : edges) {
            std::string colorName = (edge.color == 0) ? "Cm" : "Cp";
            std::cout << "{" << edge.from << "->" << edge.to << "," << colorName << "} ";
        }
        std::cout << "\n\n";
    }
    
    // Generate the full Mathematica format
    std::string mathematicaCode = generateMathematicaBCDFormat(test_data);
    std::cout << "=== Copy this into AutoWightman ===\n";
    std::cout << mathematicaCode << std::endl;
    
    return 0;
}