CXX := g++ -std=c++17
CPPFLAGS := -Wall
OBJS := single_buffer buffer_swapping circular_buffer

all: ${OBJS}

%: %.cpp
	${CXX} ${CPPFLAGS} -o $@ $< -lpthread -lboost_system -lboost_program_options
	objdump -S --disassemble $@ > $@.dump

clean:
	@rm *.dump ${OBJS}