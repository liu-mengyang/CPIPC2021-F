CPP = g++
DEBUG = -D_MACRO
TARGET = ex1

all: clean $(TARGET)

$(TARGET): 1b.o 2b.o 3b.o
	$(CPP) -o 1b 1b.o
	$(CPP) -o 2b 2b.o
	$(CPP) -o 3b 3b.o
	del *.o

1b.o: 1b.cpp
	$(CPP) -c 1b.cpp

2b.o: 2b.cpp
	$(CPP) -c 2b.cpp

3b.o: 3b.cpp
	$(CPP) -c 3b.cpp

clean:
	del *.o *.exe