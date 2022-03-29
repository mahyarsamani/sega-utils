#include "graph_reader.hh"

int main(int argc, char** argv) {
    if (argc != 4) {
        throw -1;
    }

    std::string graph_file_name = argv[1];
    std::string outdir = argv[2];
    int num_mpus = std::stoi(argv[3]);

    GraphReader graph_reader(graph_file_name, outdir, num_mpus);

    graph_reader.createBinaryFiles();

    return 0;
}