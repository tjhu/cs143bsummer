CXX := g++ 
CPPFLAGS := -Wall
OBJS := single_buffer buffer_swapping

all: ${OBJS}

%: %.cpp
	${CXX} ${CPPFLAGS} -o $@ $< -lpthread
	objdump -S --disassemble $@ > $@.dump

clean:
	@rm *.dump ${OBJS}