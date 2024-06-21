
#include "graph_separator.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

bool compareSrc(Edge lhs, Edge rhs)
{
    if (lhs._src < rhs._src) {
        return true;
    } else if (lhs._src == rhs._src) {
        return compareDst(lhs, rhs);
    } else {
        return false;
    }
}

bool compareDst(Edge lhs, Edge rhs) {
    if (lhs._dst < rhs._dst) {
        return true;
    } else if (lhs._dst == rhs._dst) {
        return compareSrc(lhs, rhs);
    } else {
        return false;
    }
}

void GraphSeparator::separateGraph()
{
    std::ifstream graph(_graph);

    std::string main_name = _outdir + "/pel_main.txt";
    std::ofstream main(main_name);
    std::string mirror_name = _outdir + "/pel_mirrors.txt";
    std::ofstream mirror(mirror_name);

    std::vector<Edge> main_edges;
    int num_slices = totalSlices();
    std::vector<std::vector<Edge>> mirror_edges(num_slices * num_slices);

    int src, dst;
    double weight = 0.0;
    while (!graph.fail()) {
        if (_weighted) {
            graph >> src >> dst >> weight;
        } else {
            graph >> src >> dst;
        }
        if (src == dst) {
            continue;
        }
        if (!graph.fail()) {
            int src_id = homeId(src);
            int dst_id = homeId(dst);
            if (src_id == dst_id) {
                main_edges.emplace_back(src, dst, weight);
            } else {
                int index = src_id * num_slices + dst_id;
                mirror_edges[index].emplace_back(src, dst, weight);
            }
        }
    }
    graph.close();

    std::sort(main_edges.begin(), main_edges.end(), compareSrc);
    std::cout << "size: " << main_edges.size() << std::endl;

    for (int i = 0; i < num_slices * num_slices; i++) {
        std::sort(mirror_edges[i].begin(), mirror_edges[i].end(), compareSrc);
    }

    for (auto edge: main_edges) {
        if (_weighted) {
            main << edge._src << " " << edge._dst << " " << edge._weight << std::endl;
        } else {
            main << edge._src << " " << edge._dst << std::endl;
        }
    }

    for (int i = 0; i < num_slices * num_slices; i++) {
        for (auto edge: mirror_edges[i]) {
            if (_weighted) {
                mirror << edge._src << " " << edge._dst << " " << edge._weight << std::endl;
            } else {
                mirror << edge._src << " " << edge._dst << std::endl;
            }
        }
    }

    main.close();
    mirror.close();
    return;
}
