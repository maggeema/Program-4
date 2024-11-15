# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# Source and object files
SRCS = s1.cc s2.cc s3.cc image.cc
OBJS = $(SRCS:.cc=.o)

# Output executable
EXEC = program

# Target to build the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)

# Rule to compile .cc files to .o files
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

# Clean up build files
clean:
	rm -f $(OBJS) $(EXEC)

# Phony targets
.PHONY: clean
