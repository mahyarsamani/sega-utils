CXX=g++
TARGET=reader
OBJS=graph_reader.o load_graph.o

all : $(OBJS)
	g++ $(OBJS) -o $(TARGET)

%.o : %.cc
	$(CXX) -c $< -Iinclude

clean:
	rm $(OBJS)
	rm $(TARGET)

