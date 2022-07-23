# First networkx library is imported
# along with matplotlib
import argparse
import networkx as nx
import matplotlib.pyplot as plt


# Defining a Class
class GraphVisualization:

    def __init__(self):

        # visual is a list which stores all
        # the set of edges that constitutes a
        # graph
        self.visual = []

    # addEdge function inputs the vertices of an
    # edge and appends it to the visual list
    def addEdge(self, a, b):
        temp = [a, b]
        self.visual.append(temp)

    # In visualize function G is an object of
    # class Graph given by networkx G.add_edges_from(visual)
    # creates a graph with a given list
    # nx.draw_networkx(G) - plots the graph
    # plt.show() - displays the graph
    def visualize(self):
        G = nx.DiGraph()
        G.add_edges_from(self.visual)
        nx.draw_networkx(G, arrows=True)
        plt.show()

def get_inputs():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("graph_file", type=str)
    args = argparser.parse_args()
    return args.graph_file

if __name__ == "__main__":
    graph_name = get_inputs()
    G = GraphVisualization()
    with open(graph_name, "r") as graph_file:
        for line in graph_file.readlines():
            src_id, dst_id = line.split()
            src_id = int(src_id)
            dst_id = int(dst_id)
            G.addEdge(src_id, dst_id)
    G.visualize()
