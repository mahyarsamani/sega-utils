#include "graph_reader.hh"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Help prompt for this binary:" << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[graph_path: str] [weighted: bool] [num_gpts: int] "
        << "[intlv_size: int] [num_gpts_per_edge_binary] [out_path: str]" << std::endl;
        return 0;
    } else if (argc != 7) {
        std::cout << "Wrong number of input arguments: " << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[graph_path: str] [weighted: bool] [num_gpts: int] "
        << "[intlv_size: int] [num_gpts_per_edge_binary] [out_path: str]" << std::endl;
        return 0;
    }

    std::string graph_path = argv[1];
    bool weighted = argv[2] == "true";
    int num_nodes = std::stoi(argv[3]);
    int intlv_size = std::stoi(argv[4]);
    int nodes_per_edge_binary = std::stoi(argv[5]);
    std::string out_path = argv[6];

    GraphReader graph_reader(graph_path, weighted, intlv_size,
                    num_nodes, nodes_per_edge_binary, out_path);

    graph_reader.createBinaryFiles();

    return 0;
}
