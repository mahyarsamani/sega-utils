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

#ifndef __GRAPH_READER_HH__
#define __GRAPH_READER_HH__

#include <cstdint>
#include <fstream>
#include <string>

struct __attribute__ ((packed)) WorkListItem
{
    uint32_t tempProp: 32;
    uint32_t prop: 32;
    uint32_t edgeIndex: 32;
    uint32_t degree: 30;
    bool activeNow: 1;
    bool activeFuture: 1;

    WorkListItem(uint32_t temp_prop, uint32_t prop,
                uint32_t degree, uint32_t edge_index,
                bool active_now, bool active_future):
        tempProp(temp_prop),
        prop(prop),
        edgeIndex(edge_index),
        degree(degree),
        activeNow(active_now),
        activeFuture(active_future)
    {}
    std::string to_string() {
      std::string ret = "";
      ret += "WorkListItem{temp_prop: " + std::to_string(tempProp) +
                ", prop: " + std::to_string(prop) +
                ", degree: " + std::to_string(degree) +
                ", edgeIndex: " + std::to_string(edgeIndex) +
                ", activeNow: " + (activeNow ? "true" : "false") +
                ", activeFuture: " + (activeFuture ? "true" : "false") + "}";
      return ret;
    }
};

struct __attribute__ ((packed)) Edge
{
    uint16_t weight : 16;
    uint64_t neighbor : 48;

    Edge(uint64_t weight, uint64_t neighbor):
        weight(weight),
        neighbor(neighbor)
    {}

    std::string to_string() {
      std::string ret = "";
      ret += "Edge{weight: " + std::to_string(weight) + ", neighbord: " +
              std::to_string(neighbor) + "}";
      return ret;
    }
};

class GraphReader
{
  private:
    std::string graphFileName;
    bool isWeighted;
    int interleavingSize;
    std::string outdir;
    int numMPUs;

    uint64_t numEdgesRead;

    uint64_t numVerticesRead;
    uint64_t numHolesFilled;

    int getMPUId(int vid);

  public:

    GraphReader(std::string graph_file_name, bool is_weighted,
                int intlv_size, std::string outdir, int num_mpus);

    void createBinaryFiles();
};

#endif // __GRAPH_READER_HH__
