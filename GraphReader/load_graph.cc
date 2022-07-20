#include "graph_reader.hh"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 6) {
        throw -1;
    }

    std::string graph_file_name = argv[1];
    std::string mode = argv[2];
    int atom_size = std::stoi(argv[3]);
    std::string outdir = argv[4];
    int num_mpus = std::stoi(argv[5]);

    bool is_weighted;
    if (mode == "weighted") {
        is_weighted = true;
    } else {
        is_weighted = false;
    }

    GraphReader graph_reader(graph_file_name, is_weighted, atom_size, outdir, num_mpus);

    graph_reader.createBinaryFiles();

    return 0;
}