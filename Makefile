CXX=g++
TARGET=loader
OBJS=graph_reader.o load_graph.o

all : $(OBJS)
	g++ $(OBJS) -o $(TARGET)

%.cc : %.o
	$(CXX) -c $< -Lgraph_reader.hh


