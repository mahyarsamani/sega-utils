#!/bin/python
import argparse
from math import ceil

if __name__ == "__main__":
    argparser = argparse.ArgumentParser()

    argparser.add_argument("input_graph_file", type=str)
    argparser.add_argument("output_graph_file", type=str)

    args = argparser.parse_args()

    edges = []
    with open(args.input_graph_file, "r") as input_graph_file:
        degrees = {}
        for line in input_graph_file.readlines():
            if line.startswith("#"):
                continue
            src_id = line.split()[0]
            if src_id in degrees:
                degrees[src_id] += 1
            else:
                degrees[src_id] = 1
        degrees = [(src, deg) for src, deg in degrees.items()]
        degrees = sorted(degrees, reverse=True, key=lambda x: x[1])

        even_id = 0
        odd_id = 1
        max_translation_id = 0
        translate_table = {}
        for i in range(ceil(len(degrees)/2)):
            big_src = degrees[i][0]
            little_src = degrees[len(degrees)-1-i][0]
            translate_table[big_src] = even_id
            max_translation_id = even_id
            even_id += 2
            if len(degrees)-1-i > i:
                translate_table[little_src] = odd_id
                max_translation_id = odd_id
                odd_id += 2
        first_avail_id = max_translation_id + 1

        input_graph_file.seek(0)
        for line in input_graph_file.readlines():
            if line.startswith("#"):
                continue
            src_id, dst_id = line.split()[:2]
            new_src_id = translate_table[src_id]
            if dst_id in translate_table:
                new_dst_id = translate_table[dst_id]
            else:
                new_dst_id = first_avail_id
                translate_table[dst_id] = new_dst_id
                first_avail_id += 1
            edges.append((new_src_id, new_dst_id))

    edges =  list(dict.fromkeys(edges))
    edges = sorted(edges, key = lambda x: (x[0], x[1]))
    min_id = edges[0][0]
    edges = [(e[0]-min_id, e[1]-min_id) for e in edges if e[0] != e[1]]

    with open(args.output_graph_file, "w") as output_graph_file:
        for src_id, dst_id in edges:
            output_graph_file.write(f"{src_id} {dst_id}\n")
