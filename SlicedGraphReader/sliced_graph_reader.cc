/*
 * Copyright (c) 2020 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sliced_graph_reader.hh"

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#define INF_VAL 4294967295

SlicedGraphReader::SlicedGraphReader(std::string graph_path,
                                    std::string main_path,
                                    std::string mirrors_path,
                                    bool is_weighted,
                                    int num_mpus,
                                    int intlv_size,
                                    int vertices_per_slice,
                                    std::string outdir):
    graph(graph_path), isWeighted(is_weighted), numMPUs(num_mpus),
    interleavingSize(intlv_size), verticesPerSlice(vertices_per_slice),
    outdir(outdir), numEdgesRead(0), numVerticesRead(0),
    numHolesFilled(0), numMirrorRead(0)
{
    main.open(main_path);
    mirrors.open(mirrors_path);
}

int
SlicedGraphReader::getMaxDstId()
{
    int src_id;
    int dst_id;
    int weight;
    int max_dst_id = -1;

    std::ifstream graph_file;
    graph_file.open(graph);
    while (!graph_file.fail()) {
        if (isWeighted) {
            graph_file >> src_id >> dst_id >> weight;
        } else {
            graph_file >> src_id >> dst_id;
            weight = 0;
        }
        if (dst_id > max_dst_id) {
            max_dst_id = dst_id;
        }
    }
    graph_file.close();
    return max_dst_id;
}

int
SlicedGraphReader::getMPUId(int vid)
{
    int num_edge_binaries = numMPUs / 2;
    return (vid / (interleavingSize / sizeof(WorkListItem))) % num_edge_binaries;
}

int
SlicedGraphReader::getSliceId(int vid)
{
    return std::floor(vid / verticesPerSlice);
}

void
SlicedGraphReader::createBinaryFiles()
{
    std::ofstream vertex_binary(outdir + "/vertices",
                                std::ios::binary | std::ios::out);
    std::ofstream mirrors_binary(outdir + "/mirrors",
                                std::ios::binary | std::ios::out);
    std::ofstream map_binary(outdir + "/mirrors_map",
                                std::ios::binary | std::ios::out);
    std::vector<std::ofstream> edge_binaries;
    std::string base_edge_file_name = outdir + "/edgelist";
    int num_edge_binaries = numMPUs / 2;
    for (int i = 0; i < num_edge_binaries; i++) {
        edge_binaries.emplace_back(
            base_edge_file_name + "_" + std::to_string(i),
            std::ios::binary | std::ios::out);
    }

    int curr_src_id = -1;
    uint32_t curr_edge_index[num_edge_binaries] = {0};
    uint32_t curr_num_edges = 0;
    int max_dst_id = getMaxDstId();
    int src_id, dst_id, weight;
    while (!main.fail()) {
        if (isWeighted) {
            main >> src_id >> dst_id >> weight;
        } else {
            main >> src_id >> dst_id;
            weight = rand() % 32 + 1;
        }
        // Found new src_id or end of the file
        if (src_id != curr_src_id || main.fail()) {
            if (curr_src_id != -1) {
                int mpu_id = getMPUId(curr_src_id);

                WorkListItem wl = {INF_VAL, INF_VAL, curr_num_edges,
                                curr_edge_index[mpu_id], false, false};
                vertex_binary.write((char*) &wl, sizeof(WorkListItem));
                numVerticesRead++;

                curr_edge_index[mpu_id] += curr_num_edges;
                curr_num_edges = 0;
            }

            for (int id = curr_src_id + 1; id < src_id; id++) {
                int mpu_id = getMPUId(id);
                WorkListItem wl = {INF_VAL, INF_VAL, 0,
                            curr_edge_index[mpu_id], false, false};
                vertex_binary.write((char*) &wl, sizeof(WorkListItem));
                numHolesFilled++;
            }

            // If main.fail() it means that we are at the end of the
            // file. No need to rewrite last edge to the edgelist. If
            // !main.fail(), then it means we need to append the first edge
            // for the new vertex here.
            if (!main.fail()) {
                curr_src_id = src_id;
                int mpu_id = getMPUId(curr_src_id);

                uint64_t dst_addr = dst_id * sizeof(WorkListItem);
                Edge e(weight, dst_addr);
                edge_binaries[mpu_id].write((char*) &e, sizeof(Edge));
                curr_num_edges++;
                numEdgesRead++;
            }
        } // New edge for the same src_id
        else {
            int mpu_id = getMPUId(curr_src_id);

            uint64_t dst_addr = dst_id * sizeof(WorkListItem);
            Edge e(weight, dst_addr);
            edge_binaries[mpu_id].write((char*) &e, sizeof(Edge));
            curr_num_edges++;
            numEdgesRead++;
        }
    }

    for (int id = curr_src_id + 1; id <= max_dst_id; id++) {
        int mpu_id = getMPUId(id);
        WorkListItem wl = {INF_VAL, INF_VAL, 0,
                    curr_edge_index[mpu_id], false, false};
        vertex_binary.write((char*) &wl, sizeof(WorkListItem));
        numHolesFilled++;
    }
    vertex_binary.close();

    curr_src_id = -1;
    int curr_src_slice = 0;
    int curr_dst_slice = -1;
    int bytes_written_to_mirror = 0;
    int total_slices = std::ceil((float)(max_dst_id + 1) / verticesPerSlice);
    while (!mirrors.fail()) {
        if (isWeighted) {
            mirrors >> src_id >> dst_id >> weight;
        } else {
            mirrors >> src_id >> dst_id;
            weight = rand() % 32 + 1;
        }
        int src_slice = getSliceId(src_id);
        int dst_slice = getSliceId(dst_id);
        if ((src_slice != curr_src_slice) || (dst_slice != curr_dst_slice)) {
            int num_fills = (total_slices - curr_dst_slice)
                            + ((src_slice - curr_src_slice - 1) * total_slices)
                            + (dst_slice);
            for (int i = 0; i < num_fills; i++) {
                map_binary.write((char*) &bytes_written_to_mirror, sizeof(int));
            }
            curr_src_slice = src_slice;
            curr_dst_slice = dst_slice;
        }
        if (src_id != curr_src_id || mirrors.fail()) {
            if (curr_src_id != -1) {
                int mpu_id = getMPUId(curr_src_id);
                MirrorVertex mv = {curr_src_id, INF_VAL, curr_num_edges,
                                curr_edge_index[mpu_id], false, false};
                mirrors_binary.write((char*) &mv, sizeof(MirrorVertex));
                curr_edge_index[mpu_id] += curr_num_edges;
                curr_num_edges = 0;
                numMirrorRead++;
                bytes_written_to_mirror += sizeof(MirrorVertex);
            }
            if (!mirrors.fail()) {
                curr_src_id = src_id;
                int mpu_id = getMPUId(curr_src_id);

                uint64_t dst_addr = dst_id * sizeof(WorkListItem);
                Edge e(weight, dst_addr);
                edge_binaries[mpu_id].write((char*) &e, sizeof(Edge));
                curr_num_edges++;
                numEdgesRead++;
            }
        } else {
            int mpu_id = getMPUId(curr_src_id);

            uint64_t dst_addr = dst_id * sizeof(WorkListItem);
            Edge e(weight, dst_addr);
            edge_binaries[mpu_id].write((char*) &e, sizeof(Edge));
            curr_num_edges++;
            numEdgesRead++;
        }
    }
    int num_fills = (total_slices - curr_dst_slice)
                    + ((total_slices - curr_src_slice - 2) * total_slices)
                    + (total_slices - 1) + 1;
    for (int i = 0; i < num_fills; i++) {
        map_binary.write((char*) &bytes_written_to_mirror, sizeof(int));
    }

    mirrors_binary.close();
    map_binary.close();
    for (auto &edge_binary: edge_binaries) {
        edge_binary.close();
    }
    std::cout << "Read " << numEdgesRead << " edges, read " << numVerticesRead
    << " vertices, filled " << numHolesFilled << " holes, and read "
    << numMirrorRead << " mirror vertices." << std::endl;
}
