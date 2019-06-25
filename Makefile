CXX := g++ -Wall
OBJS = shared_var

all: ${OBJS}

%: %.cpp
	${CXX} -o $@ $< -lpthread

clean:
	@rm *.o ${OBJS}