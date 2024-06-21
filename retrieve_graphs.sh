#!/bin/bash

# Create the graphs directory if it doesn't exist
echo "Creating graphs directory if it doesn't exist"
mkdir -p graphs

# Download the graphs
echo "Creating a directory for roadusa if it doesn't exist"
mkdir -p graphs/roadusa
echo "Changing directory to graphs/roadusa"
cd graphs/roadusa
echo "Downloading roadusa graph into graphs/roadusa/el.txt.gz"
wget http://www.dis.uniroma1.it/challenge9/data/USA-road-d/USA-road-d.USA.gr.gz -O el.txt.gz
echo "Unzipping roadusa graph"
gunzip el.txt.gz
echo "Removing comment lines and 'a' from graphs/roadusa/el.txt"
sed -i '/^a/!d' el.txt
sed -i 's/a //g' el.txt
echo "Done retrieving roadusa graph. It is ready to be passed to graph_sorter.py"
echo "Changing directory to the parent directory"
cd ../..

echo "Creating a directory for live-journal if it doesn't exist"
mkdir -p graphs/live-journal
echo "Changing directory to graphs/live-journal"
cd graphs/live-journal
echo "Downloading live-journal graph into graphs/roadusa/el.txt.gz"
wget https://snap.stanford.edu/data/soc-LiveJournal1.txt.gz -O el.txt.gz
echo "Unzipping live-journal graph"
gunzip el.txt.gz
echo "Done retrieving live-journal graph. It is ready to be passed to graph_sorter.py"
echo "Changing directory to the parent directory"
cd ../..

echo "Creating a directory for twitter if it doesn't exist"
mkdir -p graphs/twitter
cd graphs/twitter
echo "Downloading part 0 of the twitter graph"
wget https://github.com/ANLAB-KAIST/traces/releases/download/twitter_rv.net/twitter_rv.net.00.gz -O el.txt.gz_p0
echo "Downloading part 1 of the twitter graph"
wget https://github.com/ANLAB-KAIST/traces/releases/download/twitter_rv.net/twitter_rv.net.01.gz -O el.txt.gz_p1
echo "Downloading part 2 of the twitter graph"
wget https://github.com/ANLAB-KAIST/traces/releases/download/twitter_rv.net/twitter_rv.net.02.gz -O el.txt.gz_p2
echo "Downloading part 3 of the twitter graph"
wget https://github.com/ANLAB-KAIST/traces/releases/download/twitter_rv.net/twitter_rv.net.03.gz -O el.txt.gz_p3
echo "Unzipping the twitter graph"
gunzip -c el.txt.gz_p0 el.txt.gz_p1 el.txt.gz_p2 el.txt.gz_p3 > el.txt
echo "Removing the partial downloads"
rm el.txt.gz_p0 el.txt.gz_p1 el.txt.gz_p2 el.txt.gz_p3
cd ../..

echo "Creating a directory for friendster if it doesn't exist"
mkdir -p graphs/friendster
cd graphs/friendster
echo "Downloading friendster graph"
wget https://snap.stanford.edu/data/bigdata/communities/com-friendster.ungraph.txt.gz -O el.txt.gz
echo "Unzipping the friendster graph"
gunzip el.txt.gz
cd ../..

echo "Creating a directory for host if it doesn't exist"
mkdir -p graphs/host
cd graphs/host
echo "Downloading host graph"
wget https://data.dws.informatik.uni-mannheim.de/hyperlinkgraph/2012-08/sd-arc.gz -O el.txt.gz
echo "Unzipping the host graph"
gunzip el.txt.gz
cd ../..

echo "Clong gapbs repository"
git clone https://github.com/sbeamer/gapbs.git
echo "Changing directory to gapbs"
cd gapbs
echo "Making the binaries to use converter"
make
cd ..

echo "Creating a directory for rmat_21, rmat_22, rmat_23, rmat_24, rmat_25 if they don't exist"
mkdir -p graphs/rmat_21
mkdir -p graphs/rmat_22
mkdir -p graphs/rmat_23
mkdir -p graphs/rmat_24
mkdir -p graphs/rmat_25
echo "Creating a repository for urand if it doesn't exist"
mkdir -p graphs/urand
echo "Generating rmat_21 graph using gapbs converter"
./gapbs/converter -u 21 -k 16 -e graphs/rmat_21/el.txt &
echo "Generating rmat_22 graph using gapbs converter"
./gapbs/converter -u 22 -k 16 -e graphs/rmat_22/el.txt &
echo "Generating rmat_23 graph using gapbs converter"
./gapbs/converter -u 23 -k 16 -e graphs/rmat_23/el.txt &
echo "Generating rmat_24 graph using gapbs converter"
./gapbs/converter -u 24 -k 16 -e graphs/rmat_24/el.txt &
echo "Generating rmat_25 graph using gapbs converter"
./gapbs/converter -u 25 -k 16 -e graphs/rmat_25/el.txt &
echo "Generating urand graph using gapbs converter"
./gapbs/converter -u 27 -k 16 -e graphs/urand/el.txt &

sleep 5
echo "Waiting for all graphs to be generated"
wait
echo "Created all synthetic graphs"

echo "Removing gapbs repository"
rm -rf gapbs
