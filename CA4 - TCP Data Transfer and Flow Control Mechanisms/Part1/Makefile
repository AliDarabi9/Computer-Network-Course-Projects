
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = my_program


all: $(TARGET)


$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) -pthread


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
