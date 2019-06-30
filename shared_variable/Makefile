CXX := g++ 
CPPFLAGS := -Wall
OBJS := sequantial unsynchronized synchronized peterson

all: ${OBJS}

%: %.cpp
	${CXX} ${CPPFLAGS} -o $@ $< -lpthread
	objdump -S --disassemble $@ > $@.dump

clean:
	@rm *.dump ${OBJS}