#include <filesystem>
#include <sstream>
#include <string>


class Edge {
    private:
        int src;
        int dst;
        double weight;
        bool isWeighted;

    public:
        Edge(int src, int dst, double weight, bool isWeighted):
            src(src), dst(dst), weight(weight), isWeighted(isWeighted) {}

        int getSrc() const {
            return src;
        }

        int getDst() const {
            return dst;
        }

        double getWeight() const {
            return weight;
        }

        bool operator==(const Edge& a) const {
            return src == a.src && dst == a.dst;
        }
        // Overload the "<" operator for sorting
        bool operator<(const Edge& a) const {
            if (src == a.src) {
                if (dst == a.dst) {
                    return weight < a.weight;
                }
                return dst < a.dst;
            }
            return src < a.src;
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << src << " " << dst;
            if (isWeighted) {
                oss << " " << weight;
            }
            return oss.str();
        }
};

class GraphSorter {
    private:
        std::filesystem::path inputFilePath;
        std::filesystem::path outputFilePath;
        std::filesystem::path infoFilePath;
        bool isWeighted;


    public:
        GraphSorter(std::string input_file_path, bool
                is_weighted, std::string output_file_path):
            inputFilePath(input_file_path),
            outputFilePath(output_file_path),
            infoFilePath(output_file_path), isWeighted(is_weighted)
            {
                infoFilePath.replace_filename("info");
            }

    void sort();
};
