CXX=g++
TARGET=separator
OBJS=graph_separator.o separate_graph.o

all : $(OBJS)
	g++ $(OBJS) -o $(TARGET)

%.o : %.cc
	$(CXX) -c $< -Iinclude

clean:
	rm $(OBJS)
	rm $(TARGET)
