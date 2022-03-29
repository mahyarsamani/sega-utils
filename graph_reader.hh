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

struct WorkListItem
{
    uint32_t temp_prop;
    uint32_t prop;
    uint32_t degree;
    uint32_t edgeIndex;
};
uint8_t* workListToBytes(WorkListItem wl);

struct Edge
{
    uint64_t weight;
    uint64_t neighbor;

    Edge(uint64_t weight, uint64_t neighbor):
        weight(weight),
        neighbor(neighbor)
    {}
};
uint8_t* edgeToBytes(Edge e);

class GraphReader
{
  private:
    std::string graphFileName;
    std::string outdir;
    int numMPUs;

    std::pair<int, int> parseStringToInts(std::string line);

  public:

    GraphReader(std::string graph_file_name, std::string outdir, int num_mpus);
    ~GraphReader();

    void createBinaryFiles();
};

#endif // __GRAPH_READER_HH__
