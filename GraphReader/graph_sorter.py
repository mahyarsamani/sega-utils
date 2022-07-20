#!/bin/python
import argparse

if __name__ == "__main__":
    argparser = argparse.ArgumentParser()

    argparser.add_argument("input_graph_file", type=str)
    argparser.add_argument("output_graph_file", type=str)

    args = argparser.parse_args()

    edges = []
    with open(args.input_graph_file, "r") as input_graph_file:
        for line in input_graph_file.readlines():
            if line.startswith("#"):
                continue
            src_id, dst_id = line.split()
            src_id = int(src_id)
            dst_id = int(dst_id)
            edges.append((src_id, dst_id))

    edges = sorted(edges, key = lambda x: x[0])

    with open(args.output_graph_file, "w") as output_graph_file:
        for src_id, dst_id in edges:
            output_graph_file.write(f"{src_id} {dst_id}\n")
