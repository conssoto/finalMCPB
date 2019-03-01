OBJS = main.o Node.o  Truck.o ProblemInstance.o Reader.o Trip.o Route.o Solution.o Construction.o ReorderNodes.o RemoveNodes.o AddNodes.o
CFLAGS =	-std=c++1y #-fmax-errors=1 -ggdb #-ggdb for debug

all: MCPB

MCPB: $(OBJS)
	g++ -o MCPB $(OBJS) $(CFLAGS)

%.o: %.cpp
	g++ -c $(CFLAGS) $< -o $@

clean:
	rm -f MCPB *.o

exe:
	@./MCPB ${ARGS}

