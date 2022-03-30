#include "graph_reader.hh"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 5) {
        throw -1;
    }

    std::string graph_file_name = argv[1];
    std::string mode = argv[2];
    std::string outdir = argv[3];
    int num_mpus = std::stoi(argv[4]);

    if (std::getenv("LOADER_LOG") != "true") {
        std::clog.setstate(std::ios::failbit);
    }

    bool is_weighted;
    if (mode == "weighted") {
        is_weighted = true;
    } else {
        is_weighted = false;
    }

    GraphReader graph_reader(graph_file_name, is_weighted, outdir, num_mpus);

    graph_reader.createBinaryFiles();

    return 0;
}