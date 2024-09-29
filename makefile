# Makefile for CDCL Solver

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra

# Source files
SRC = CDCL_tree_threading.cpp

# Executable name
TARGET = cdcl_solver

# Input file
INPUT = input_test.txt

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the program with input.txt
run: $(TARGET)
	time ./$(TARGET) < $(INPUT)

# Clean up
clean:
	rm -f $(TARGET)
