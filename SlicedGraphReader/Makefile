CXX=g++
TARGET=sliced-reader
OBJS=sliced_graph_reader.o load_graph.o

all : $(OBJS)
	g++ $(OBJS) -o $(TARGET)

%.o : %.cc
	$(CXX) -c $< -Iinclude

clean:
	rm $(OBJS)
	rm $(TARGET)

