CC:=g++
CC:=bear --append -- $(CC)

CXXFLAGS= \
	-Wall \
	-Wshadow \
	-Wextra \
	-pedantic \
	# -O3 \

DEBUGFLAGS= \
	-g3 \
	-D_GLIBCXX_DEBUG \
	-D_GLIBCXX_DEBUG_PEDANTIC \
	-fsanitize=address,undefined,leak \
	-DDEBUG \
	-DDEBUGPRINT \
	# -g \
	# -fsanitize=float-divide-by-zero \
	# -fsanitize=float-cast-overflow \

.PHONY: all

fractal.e: fractal.o
	$(CC) fractal.o -o fractal.e

noise.e: noise_ispc.o noise_serial.o noise.o
	$(CC) noise.o noise_serial.o noise_ispc.o -o noise.e

%.o: %.cpp
	$(CC) $*.cpp -c -o $*.o

%_ispc.o: %.ispc
	ispc --target=host --pic $*.ispc -o $*_ispc.o -h $*_ispc.h


clean:
	rm *.o *.e
