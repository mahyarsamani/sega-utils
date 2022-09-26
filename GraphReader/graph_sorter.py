#!/bin/python
import argparse
from itertools import groupby

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

    edges = sorted(edges, key = lambda x: (x[0], x[1]))

    edges =  list(dict.fromkeys(edges))
    edges = [e for e in edges if e[0] != e[1]]
    
    with open(args.output_graph_file, "w") as output_graph_file:
        for src_id, dst_id in edges:
            output_graph_file.write(f"{src_id} {dst_id}\n")
