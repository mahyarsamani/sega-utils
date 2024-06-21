#!/bin/bash

# Usage function
usage() {
    echo "Usage: $0 [-g|--graphs <comma separated graph names>]"
    echo "Example: $0 -g roadusa,live-journal"
    exit 1
}

# Define the graph names and whether they are weighted or not
declare -A graph_names
graph_names=(["roadusa"]=1 ["live-journal"]=0 ["twitter"]=0 ["friendster"]=0 ["host"]=0 ["urand"]=0 ["rmat_21"]=0 ["rmat_22"]=0 ["rmat_23"]=0 ["rmat_24"]=0 ["rmat_25"]=0)

# Set the default value of user_graphs to be all the graphs
user_graphs=("${!graph_names[@]}")

# Parse command line options
TEMP=$(getopt -o g:h --long graphs:,help -n 'script.sh' -- "$@")
eval set -- "$TEMP"

# Extract options and their arguments into variables
while true ; do
    case "$1" in
        -g|--graphs)
            IFS=',' read -ra user_graphs <<< "$2"
            shift 2 ;;
        -h|--help)
            usage
            ;;
        --) shift ; break ;;
        *) echo "Internal error!" ; exit 1 ;;
    esac
done

echo "Starting preprocessing..."

# Loop over the user specified graph names
for user_graph in "${user_graphs[@]}"; do
    # Check if the user specified graph name is valid
    if [[ -n ${graph_names[$user_graph]} ]]; then
        echo "Processing graph: ${user_graph}"
        # Check if the file exists before sorting
        if [[ -f "graphs/${user_graph}/el.txt" ]]; then
            # Sort the file and generate the output file
            ./GraphSorter/sorter "graphs/${user_graph}/el.txt" ${graph_names[$user_graph]} "graphs/${user_graph}/sel.txt" &
        else
            echo "File does not exist: graphs/${user_graph}/el.txt"
        fi
    else
        echo "Invalid graph name: ${user_graph}"
    fi
done

sleep 3
echo "Waiting for all the processes to finish..."
wait
echo "Script completed."
