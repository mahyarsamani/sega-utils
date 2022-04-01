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

#include "graph_reader.hh"

#include <iostream>
#include <vector>

#define MEMORY_ATOM_SIZE 64
#define INF_VAL 4294967295

GraphReader::GraphReader(std::string graph_file_name,
                        bool is_weighted,
                        std::string outdir,
                        int num_mpus):
    graphFileName(graph_file_name),
    isWeighted(is_weighted),
    outdir(outdir),
    numMPUs(num_mpus),
    numEdgesRead(0),
    numVerticesRead(0),
    numHolesFilled(0)
{}

void
GraphReader::createBinaryFiles()
{
    std::ifstream graph_file;
    graph_file.open(graphFileName);

    std::ofstream vertex_binary;
    std::string vertex_file_name = outdir + "/vertices";
    vertex_binary.open(vertex_file_name, std::ios::binary | std::ios::out);

    std::vector<std::ofstream> edge_binaries;
    std::string base_edge_file_name = outdir + "/edgelist";
    for (int i = 0; i < numMPUs; i++) {
        edge_binaries.emplace_back(
            base_edge_file_name + "_" + std::to_string(i),
            std::ios::binary | std::ios::out);
    }

    int curr_src_id = -1;
    uint32_t curr_edge_index[numMPUs] = {0};
    uint32_t curr_num_edges = 0;

    int max_dst_id = -1;
    int src_id, dst_id, weight;
    while(!graph_file.fail()) {
        if (isWeighted) {
            graph_file >> src_id >> dst_id >> weight;
        } else {
            graph_file >> src_id >> dst_id;
            weight = 0;
        }
        // Found new src_id or end of the file
        if (src_id != curr_src_id || graph_file.fail()) {
            if (curr_src_id != -1) {
                int mpu_id = (curr_src_id /
                            (MEMORY_ATOM_SIZE / sizeof(WorkListItem)))
                            % numMPUs;
                WorkListItem wl = {INF_VAL, INF_VAL, curr_num_edges,
                                curr_edge_index[mpu_id]};
                vertex_binary.write((char*) &wl, sizeof(WorkListItem));
                numVerticesRead++;

                curr_edge_index[mpu_id] += curr_num_edges;
                curr_num_edges = 0;

                for (int id = curr_src_id + 1; id < src_id; id++) {
                    int mpu_id = (id /
                                (MEMORY_ATOM_SIZE / (sizeof(WorkListItem))))
                                % numMPUs;
                    WorkListItem wl = {INF_VAL, INF_VAL, 0,
                                curr_edge_index[mpu_id]};
                    vertex_binary.write((char*) &wl, sizeof(WorkListItem));
                    numHolesFilled++;
                }
            }

            // If graph_file.fail() it means that we are at the end of the
            // file. No need to rewrite last edge to the edgelist.
            if (!graph_file.fail()) {
                curr_src_id = src_id;
                int mpu_id = (curr_src_id /
                            (MEMORY_ATOM_SIZE / sizeof(WorkListItem)))
                            % numMPUs;

                uint64_t dst_addr = dst_id * sizeof(WorkListItem);
                Edge e(weight, dst_addr);
                edge_binaries[mpu_id].write((char*) &e, sizeof(Edge));
                curr_num_edges++;
                numEdgesRead++;
            }
        } // New edge for the same src_id
        else {
            int mpu_id = (curr_src_id /
                        (MEMORY_ATOM_SIZE / sizeof(WorkListItem)))
                        % numMPUs;

            uint64_t dst_addr = dst_id * sizeof(WorkListItem);
            Edge e(weight, dst_addr);
            edge_binaries[mpu_id].write((char*) &e, sizeof(Edge));
            curr_num_edges++;
            numEdgesRead++;
        }
    }

    for (int id = curr_src_id + 1; id <= max_dst_id; id++) {
        int mpu_id = (id /
                    (MEMORY_ATOM_SIZE / (sizeof(WorkListItem))))
                    % numMPUs;
        WorkListItem wl = {INF_VAL, INF_VAL, 0,
                    curr_edge_index[mpu_id]};
        vertex_binary.write((char*) &wl, sizeof(WorkListItem));
        numHolesFilled++;
    }

    vertex_binary.close();
    for (auto &edge_binary: edge_binaries) {
        edge_binary.close();
    }

    std::cout << "Read " << numEdgesRead << " edges, read " << numVerticesRead
    << " vertices, and filled " << numHolesFilled << " holes." << std::endl;
}

