#include "graph_separator.hh"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc == 1) {
        std::cout << "Help prompt for this binary:" << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[graph_path: str] [outdir: str] [weighted: bool] "
        << "[total_vertices: int] [vertices_per_slice: int]" << std::endl;
        return 0;
    } else if (argc != 6) {
        std::cout << "Wrong number of inputs!" << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[graph_path: str] [outdir: str] [weighted: {weighted, unweighted}] "
        << "[total_vertices: int] [vertices_per_slice: int]" << std::endl;
        return 0;
    }

    std::string graph = argv[1];
    std::string outdir = argv[2];
    bool is_weighted = std::stoi(argv[3]);
    int total_vertices = std::atoi(argv[4]);
    int vertices_per_slice = std::atoi(argv[5]);

    GraphSeparator separator(graph, outdir, is_weighted, total_vertices, vertices_per_slice);
    separator.separateGraph();
    return 0;
}
