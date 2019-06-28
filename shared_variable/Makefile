CXX := g++ 
CPPFLAGS := -Wall
OBJS := sequantial unsynchronized synchronized peterson

all: ${OBJS}

%: %.cpp
	${CXX} ${CPPFLAGS} -o $@ $< -lpthread

clean:
	@rm *.o ${OBJS}