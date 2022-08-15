import re
import json
import argparse

def get_inputs():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("simout", type=str)
    argparser.add_argument("verified", type=str)
    args = argparser.parse_args()
    return args.simout, args.verified

if __name__ == "__main__":
    test, truth = get_inputs()

    truth_data = None
    parsed_simout = {}
    with open(test, "r") as test_file:
        for line in test_file.readlines()[13:-1]:
            vid_base = int(int(re.search("WorkListItem\[(.+?)\]", line).group(1)) / 16)
            vid_offset = int(re.search("\]\[(.+?)\]", line).group(1))
            prop = int(re.search(", prop: (.+?),", line).group(1))
            parsed_simout[vid_base + vid_offset] = prop

    with open(truth, "r") as truth_file:
        truth_data = json.load(truth_file)

    for key, value in parsed_simout.items():
        if truth_data[key] != 1000:
            if value < truth_data[key]:
                print(f"smaller, vid: {key}, truth: {truth_data[key]}, test: {value}")
            if value > truth_data[key]:
                print(f"larger, vid: {key}, truth: {truth_data[key]}, test: {value}")