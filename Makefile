CXX = g++
CXXFLAGS = -Wall -std=c++11

TARGET = cpu
SRC = cpu.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

test:
	./$(TARGET) test1.txt out1.txt
	./$(TARGET) test2.txt out2.txt

clean:
	rm -f $(TARGET) *.o out1.txt out2.txt

