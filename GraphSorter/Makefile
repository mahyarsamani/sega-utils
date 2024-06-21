CXX=g++
TARGET=sorter
OBJS=graph_sorter.o sort_graph.o

all : $(OBJS)
	g++ $(OBJS) -o $(TARGET)

%.o : %.cc
	$(CXX) -c $< -Iinclude

clean:
	rm $(OBJS)
	rm $(TARGET)
