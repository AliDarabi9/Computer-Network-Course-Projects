# Variables
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = my_program

# Default target
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) -pthread

# Compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
