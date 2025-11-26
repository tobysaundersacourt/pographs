# Fast Partially-Ordered Graph Isomorphisms, Arithmetic, and Visualizations

This repository contains a C++ implementation of algorithms for computing canonical labels of partially-ordered graphs, specifically designed for applications in mathematical physics with scattering diagrams. The program efficiently generates graph coefficients and provides a data format for doing arithmetic operations over partially-ordered graphs, with output compatible with Mathematica for further analysis and clear visualization.

## Installation and Compilation

### Installing g++

If you don't have g++ installed on your system, follow these instructions:

**macOS:**
```bash
# Install Xcode command line tools
xcode-select --install
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install g++
```

**Fedora/CentOS/RHEL:**
```bash
sudo dnf install gcc-c++
# or for older versions:
sudo yum install gcc-c++
```

**Windows:**
- Install [MinGW-w64](https://www.mingw-w64.org/) or [MSYS2](https://www.msys2.org/)
- Or use [Visual Studio](https://visualstudio.microsoft.com/) with C++ support

As an example of how to use this isomorphism program, we provide `chen-strichartz_run.cpp`, which computes Chen-Strichartz coefficients for scattering diagrams up to a specified degree in Planck's constant.

# Chen-Strichartz Formula for the Magnus Expansion

### Compilation

```bash
g++ -std=c++17 -o chen-strichartz_run chen-strichartz_run.cpp graph_visualizer.cpp
```

## Usage

### Basic usage (terminal output):
```bash
./chen-strichartz_run --degree 2
./chen-strichartz_run --degree 3
```

### With Mathematica BCD format output to file:
```bash
./chen-strichartz_run --degree 2 --mathematica output.m
./chen-strichartz_run --degree 3 --mathematica output.m
```

### Additional options:
```bash
# Specify loop order (default: 1)
./chen-strichartz_run --degree 3 --loop-order 2

# Enable/disable plus-minus symmetry (default: true)
./chen-strichartz_run --degree 2 --bicolored false

# Combined example
./chen-strichartz_run --degree 4 --loop-order 1 --bicolored true --mathematica output.m
```

## Output

- **Terminal**: 
  - Coefficients with graph codes (both undiscounted and discounted)
  - Bicolored mathematical expressions with colored edges (red/blue)
- **Mathematica files**: BCD format for import into notebooks:
  ```mathematica
  myBCDgraphs = {
    {coefficient, {{from -> to, Cm}, {from -> to, Cp}, ...}},
    ...
  };
  views[myBCDgraphs]
  ```

## Mathematica Integration

To use the generated `.m` files in Mathematica notebooks:

```mathematica
(* Define colors for edges *)
Cp := RGBColor[220/255, 5/255, 5/255]; Cm := 
 RGBColor[5/255, 60/255, 240/255];

(* Function to visualize graphs with colored edges *)
view[graph_] := 
  EdgeTaggedGraph[
   MapThread[Style, {edges = graph[[All, 1]], edges = graph[[All, 2]]}],
    VertexLabels -> "Name"];
views[BCDgraphs_] := 
  Table[{item[[1]], view@item[[2]]}, {item, BCDgraphs}];

(* Set directory to your graphs_colored folder *)
SetDirectory["/Users/ts/graphs_colored"];

(* Load the generated file *)
Get[{NAME OF OUTPUT MATHEMATICA '.m' FILE}];

(* The myBCDgraphs variable is now available *)
(* views[myBCDgraphs] is automatically called *)
```

There is also a sample notebook `visualization.nb` included for reference and quick testing.

## Files

- `chen-strichartz_run.cpp` - Main program
- `graph_visualizer.cpp` - Graph parsing and visualization functions  
- `graph_visualizer.hpp` - Header file for visualization functions

# TODO

- [ ] Optimize memory usage
- [ ] Re-use computations for efficiency 
- [ ] Implement graph databases storage for caching big reusable computations and for later analysis
- [ ] Add parallel processing support
- [ ] Make sure algorithms, data structures, and keyword (those affecting efficiency, such as inline, const, &) usage is optimal
- [ ] Create detailed documentation for each overtly-user-facing function in the codebase

# Contact

For questions, suggestions, or collaboration opportunities, please email sactoa(at)gmail(dot)com.
