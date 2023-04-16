
#ifndef __GRAPH_SEPARATOR_HH__
#define __GRAPH_SEPARATOR_HH__

#include <cmath>
#include <string>

struct Edge
{
    int _src;
    int _dst;
    double _weight;
    Edge(int src, int dst, double weight):
        _src(src), _dst(dst), _weight(weight)
    {}
};

bool compareSrc(Edge lhs, Edge rhs);
bool compareDst(Edge lhs, Edge rhs);

class GraphSeparator
{
    private:
        std::string _graph;
        std::string _outdir;

        bool _weighted;
        int _numTotalVertices;
        int _numVerticesPerSlice;

        int totalSlices()
        {
            double dividend = (double) _numTotalVertices / _numVerticesPerSlice;
            return std::ceil(dividend);
        }

        int homeId(int vid) {
            double dividend = (double) vid / _numVerticesPerSlice;
            return std::floor(dividend);
        }

    public:
        GraphSeparator(std::string graph, std::string outdir,
                    bool weighted, int total_v, int v_per_slice):
            _graph(graph), _outdir(outdir), _weighted(weighted),
            _numTotalVertices(total_v), _numVerticesPerSlice(v_per_slice)
        {}

        void separateGraph();
};

#endif // __GRAPH_SEPARATOR_HH__
