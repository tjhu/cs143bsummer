CXX := g++ -std=c++17 -g
CPPFLAGS := -Wall
OBJS := single_buffer buffer_swapping circular_buffer_main

all: ${OBJS}

%: %.cpp
	${CXX} ${CPPFLAGS} -o $@ $< -lpthread
	objdump -S --disassemble $@ > $@.dump

clean:
	@rm *.dump ${OBJS}