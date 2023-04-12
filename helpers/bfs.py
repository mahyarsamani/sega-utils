import json
import argparse

def read_inputs():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("graph_file", type=str)
    argparser.add_argument("truth_path", type=str)
    args = argparser.parse_args()
    return args.graph_file, args.truth_path

def process_graph(graph_file):
    curr_src_id = -1
    curr_edge_index = 0
    curr_num_edges = 0
    max_dst_id = -1

    edge_indices = []
    edges = []

    src_id = -1
    dst_id = -1

    for line in graph_file.readlines():
        src_id, dst_id = line.split()
        src_id = int(src_id)
        dst_id = int(dst_id)

        if dst_id > max_dst_id:
            max_dst_id = dst_id

        if src_id != curr_src_id:
            if curr_src_id != -1:
                edge_indices.append(curr_edge_index)
                curr_edge_index += curr_num_edges
                curr_num_edges = 0
            for _ in range(curr_src_id + 1, src_id):
                edge_indices.append(curr_edge_index)
            curr_src_id = src_id
            edges.append(dst_id)
            curr_num_edges += 1
        else:
            edges.append(dst_id)
            curr_num_edges += 1
    edge_indices.append(curr_edge_index)
    curr_edge_index += curr_num_edges

    for _ in range(src_id + 1, max_dst_id + 1):
        edge_indices.append(curr_edge_index)

    return edge_indices, edges

if __name__ == "__main__":

    graph_file_name, truth_file_path = read_inputs()

    edge_indices = []
    edges = []
    with open(graph_file_name, "r") as graph_file:
        edge_indices, edges = process_graph(graph_file)

    heights = [1000 for _ in range(len(edge_indices))]
    frontier = [0]
    heights[0] = 0

    num_traversed = 0

    while True:
        next_frontier = []
        for curr_id in frontier:
            start = edge_indices[curr_id]
            if (curr_id != len(edge_indices) - 1):
                end = edge_indices[curr_id + 1]
            else:
                end = len(edge_indices)
            curr_edges = edges[start : end]

            num_traversed += len(curr_edges)
            for edge in curr_edges:
                if heights[edge] > (heights[curr_id] + 1):
                    heights[edge] = heights[curr_id] + 1
                    next_frontier.append(edge)
        frontier = next_frontier
        if len(frontier) == 0:
            break

    print(num_traversed)
    with open(f"{truth_file_path}/truth.json", "w") as truth_file:
        json.dump(heights, truth_file)
