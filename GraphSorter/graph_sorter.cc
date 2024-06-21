#include "graph_sorter.hh"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <deque>

void GraphSorter::sort()
{
    std::deque<Edge> edges;
    std::map<int, int> degrees;

    int src_id, dst_id;
    double weight;
    std::string line;
    std::ifstream input_file(inputFilePath.string());
    while (std::getline(input_file, line)) {
        if (line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        iss >> src_id >> dst_id;
        weight = 0.0;
        if (isWeighted) {
            iss >> weight;
        }
        if (src_id == dst_id) {
            continue;  // Ignore lines where src and dst are equal
        }
        if (degrees.find(src_id) == degrees.end()) {
            degrees[src_id] = 0;
        }
        degrees[src_id]++;
        edges.emplace_back(src_id, dst_id, weight, isWeighted);
    }
    input_file.close();

    std::vector<std::pair<int, int>> degrees_vec(degrees.begin(), degrees.end());
    std::sort(degrees_vec.begin(), degrees_vec.end(),
        [] (std::pair<int, int> a, std::pair<int, int> b) {
                return a.second > b.second;
            }
    );

    int even_id = 0;
    int odd_id = 1;
    int max_translation_id = 0;
    std::map<int, int> translate_table;
    for (int i = 0; i < std::ceil(degrees_vec.size() / 2.0); ++i) {
        int big_src = degrees_vec[i].first;
        int little_src = degrees_vec[degrees_vec.size() - 1 - i].first;
        translate_table[big_src] = even_id;
        max_translation_id = even_id;
        even_id += 2;
        if (degrees_vec.size() - 1 - i > i) {
            translate_table[little_src] = odd_id;
            max_translation_id = odd_id;
            odd_id += 2;
        }
    }
    std::cout << "Max Translation ID: " << max_translation_id << std::endl;
    int first_avail_id = max_translation_id + 1;

    int len = edges.size();
    for (int i = 0; i < len; i++) {
        Edge edge = edges.front();
        edges.pop_front();
        int new_src, new_dst;
        assert(translate_table.find(edge.getSrc()) != translate_table.end());
        new_src = translate_table[edge.getSrc()];
        if (translate_table.find(edge.getDst()) != translate_table.end()) {
            new_dst = translate_table[edge.getDst()];
        } else {
            new_dst = first_avail_id;
            translate_table[edge.getDst()] = first_avail_id;
            first_avail_id++;
        }
        edges.emplace_back(new_src, new_dst, edge.getWeight(), isWeighted);
    }

    std::ofstream info_file(infoFilePath.string());
    info_file << "vertices: " << first_avail_id << std::endl;
    info_file.close();

    std::sort(edges.begin(), edges.end());

    std::ofstream output_file(outputFilePath);
    Edge last_edge(-1, -1, 0, isWeighted);
    for (const auto& e : edges) {
        if (e == last_edge) {
            continue;
        }
        output_file << e.toString() << "\n";
        last_edge = e;
    }
    output_file.close();
}