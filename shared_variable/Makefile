CXX := g++ -Wall
OBJS = sequantial unsynchronized synchronized atomic

all: ${OBJS}

%: %.cpp
	${CXX} -o $@ $< -lpthread

clean:
	@rm *.o ${OBJS}