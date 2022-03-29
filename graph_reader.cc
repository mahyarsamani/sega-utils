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

#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>

#define MEMORY_ATOM_SIZE 64
#define INF_VAL 4294967295

uint8_t*
workListToBytes(WorkListItem wl){
    int  data_size = sizeof(WorkListItem) / sizeof(uint8_t);
    uint8_t* data = new uint8_t [data_size];

    uint32_t* tempPtr = (uint32_t*) data;
    *tempPtr = wl.temp_prop;

    uint32_t* propPtr = (uint32_t*) (data + 4);
    *propPtr = wl.prop;

    uint32_t* degreePtr = (uint32_t*) (data + 8);
    *degreePtr = wl.degree;

    uint32_t* edgePtr = (uint32_t*) (data + 12);
    *edgePtr = wl.edgeIndex;

    return data;
}

uint8_t*
edgeToBytes(Edge e)
{
    int data_size = (int) ((sizeof(Edge)) / (sizeof(uint8_t)));

    uint8_t* data = new uint8_t [data_size];

    uint64_t* weightPtr = (uint64_t*) data;
    *weightPtr = e.weight;

    // data + 8 because weight: 8 bytes
    uint64_t* neighborPtr = (uint64_t*) (data + 8);
    *neighborPtr = e.neighbor;

    return data;
}

GraphReader::GraphReader(std::string graph_file_name,
                        std::string outdir,
                        int num_mpus):
    graphFileName(graph_file_name),
    outdir(outdir),
    numMPUs(num_mpus)
{}

GraphReader::~GraphReader()
{}

std::pair<int, int>
GraphReader::parseStringToInts(std::string line)
{
    std::string line_bak = line;
    std::string delimiter = " ";
    size_t pos = 0;

    int tokenIndex = 0;
    int srcId = 0;
    int dstId = 0;

    while ((pos = line.find(delimiter)) != std::string::npos) {
        int token = stoi(line.substr(0, pos));
        if (tokenIndex == 0) {
            srcId = token;
            tokenIndex++;
        } else if (tokenIndex == 1) {
            dstId = token;
            tokenIndex++;
        } else {
            std::cerr << line_bak << " " << srcId << " " << dstId << std::endl;
            throw -1;
        }
        line.erase(0, pos + delimiter.length());
    }
    return std::make_pair(srcId, dstId);
}

void
GraphReader::createBinaryFiles()
{
    std::ifstream graph_file;
    graph_file.open(graphFileName);

    std::string vertex_file_name = outdir + "/vertices";
    std::string base_edge_file_name = outdir + "/edgelist";

    std::ofstream vertex_binary;
    vertex_binary.open(vertex_file_name, std::ios::binary | std::ios::out);

    std::ofstream edge_binary;

    int curr_src_id = -1;
    uint32_t curr_edge_index[numMPUs] = {0};
    std::vector<Edge> curr_edge_list;
    curr_edge_list.clear();

    std::string line;
    while(graph_file) {
        std::getline(graph_file, line);
        int src_id, dst_id;
        std::tie(src_id, dst_id) = parseStringToInts(line);
        // Found new src_id
        if (src_id != curr_src_id) {
            // First vertex
            if (curr_src_id == -1) {
                curr_src_id = src_id;
                uint64_t dst_addr = dst_id * sizeof(WorkListItem);
                curr_edge_list.emplace_back(0, dst_addr);
            } else {
                int mpu_id = (curr_src_id /
                            (MEMORY_ATOM_SIZE / sizeof(WorkListItem)))
                            % numMPUs;
                uint32_t num_edges = curr_edge_list.size();
                WorkListItem wl = {INF_VAL, INF_VAL, num_edges,
                                    curr_edge_index[mpu_id]};
                uint8_t* wl_data = workListToBytes(wl);
                vertex_binary.write((char*) wl_data, sizeof(WorkListItem));

                uint8_t* edge_data = new uint8_t [num_edges * sizeof(Edge)];
                for (int i = 0; i < curr_edge_list.size(); i++) {
                    uint8_t* curr_edge_data = edgeToBytes(curr_edge_list[i]);
                    std::memcpy(edge_data + (i * sizeof(Edge)),
                                curr_edge_data, sizeof(Edge));
                }
                std::string edge_file_name =
                    base_edge_file_name + "_" + std::to_string(mpu_id);
                edge_binary.open(edge_file_name, std::ios::binary |
                                                    std::ios::out |
                                                    std::ios::ate);
                edge_binary.write((char*) edge_data, num_edges * sizeof(Edge));
                edge_binary.close();

                curr_edge_index[mpu_id] += num_edges;

                curr_src_id = src_id;
                curr_edge_list.clear();

                uint64_t dst_addr = dst_id * sizeof(WorkListItem);
                curr_edge_list.emplace_back(0, dst_addr);
            }
        } // New edge for the same src_id
        else {
            // TODO: replace 16 with sizeof(WorkListItem)
            uint64_t dst_addr = dst_id * sizeof(WorkListItem);
            curr_edge_list.emplace_back(0, dst_addr);
        }
    }
    vertex_binary.close();
}

