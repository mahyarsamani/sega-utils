#include "sliced_graph_reader.hh"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Help prompt for this binary:" << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[main_path: str] [mirrors_path: str] [max_id: int] "
        << "[weighted: bool] [num_gpts: int] [intlv_size: int] "
        << "[vertices_per_slice: int] [out_path: str]" << std::endl;
        return 0;
    } else if (argc != 9) {
        std::cout << "Wrong number of input arguments: " << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
        << "[main_path: str] [mirrors_path: str] [max_id: int] "
        << "[weighted: bool] [num_gpts: int] [intlv_size: int] "
        << "[vertices_per_slice: int] [out_path: str]" << std::endl;
        return 0;
    }

    std::string main_path = argv[1];
    std::string mirrors_path = argv[2];
    int max_id = std::atoi(argv[3]);
    bool weighted = argv[4] == "true";
    int num_nodes = std::stoi(argv[5]);
    int intlv_size = std::stoi(argv[6]);
    int vertices_per_slice = std::stoi(argv[7]);
    std::string out_path = argv[8];

    SlicedGraphReader graph_reader(main_path, mirrors_path, max_id, weighted,
                        num_nodes, intlv_size, vertices_per_slice, out_path);

    graph_reader.createBinaryFiles();

    return 0;
}
