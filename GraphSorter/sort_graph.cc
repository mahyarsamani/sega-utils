#include "graph_sorter.hh"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Help prompt for this binary:" << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
                  << "[input_file_path: str] [is_weighted: bool] [output_file_path: str]" << std::endl;
        return 0;
    } else if (argc != 4) {
        std::cout << "Wrong number of inputs!" << std::endl;
        std::cout << "Please provide these input arguments." << std::endl
                  << "[input_file_path: str] [is_weighted: bool] [output_file_path: str]" << std::endl;
        return 0;
    }

    std::string input_file_path = argv[1];
    bool is_weighted = std::stoi(argv[2]);
    std::string output_file_path = argv[3];

    GraphSorter sorter(input_file_path, is_weighted, output_file_path);
    sorter.sort();

    return 0;
}