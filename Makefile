CXX=g++
TARGET=loader
OBJS=graph_reader.o load_graph.o

all : $(OBJS)
	g++ $(OBJS) -o $(TARGET)

%.o : %.cc
	$(CXX) -c $< -Iinclude

install:
	cp loader ~/usr/bin

clean:
	rm $(OBJS)
	rm $(TARGET)
	rm ~/usr/bin/loader
