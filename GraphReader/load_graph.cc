#include "graph_reader.hh"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Help prompt for this binary:" << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[graph_path: str] [weighted: bool] [num_gpts: int] "
        << "[atom_size: int] [out_path: str]." << std::endl;
        return 0;
    } else if (argc != 6) {
        std::cout << "Wrong number of input arguments." << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[graph_path: str] [weighted: bool] [num_gpts: int] "
        << "[atom_size: int] [out_path: str]." << std::endl;
        return 0;
    }

    std::string graph_path = argv[2];
    bool weighted = argv[3] == "true";
    int num_nodes = std::stoi(argv[4]);
    int atom_size = std::stoi(argv[5]);
    std::string out_path = argv[6];

    GraphReader graph_reader(graph_path, weighted, atom_size, out_path, num_nodes);

    graph_reader.createBinaryFiles();

    return 0;
}
