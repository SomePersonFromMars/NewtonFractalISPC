CC:=g++
CC:=bear --append -- $(CC)

CXXFLAGS= \
	-Wall \
	-Wshadow \
	-Wextra \
	-pedantic

fractal.e: fractal.o fractal_ispc.o complex_ispc.o tasksys.o
	$(CC) fractal.o fractal_ispc.o complex_ispc.o tasksys.o -o fractal.e

fractal.o: fractal_ispc.h settings.h
fractal_ispc.h: fractal_ispc.o

%.o: %.cpp
	$(CC) $(CXXFLAGS) $*.cpp -c -o $*.o

%_ispc.o: %.ispc
	ispc --target=host --pic $*.ispc -o $*_ispc.o -h $*_ispc.h


clean:
	rm *.o *.e *_ispc.h *.ppm compile_commands.json
