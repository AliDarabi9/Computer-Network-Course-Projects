CXX = g++
CXXFLAGS = -pthread -std=c++11
TARGETS = HostA Router HostB

all: $(TARGETS)

HostA: HostA.cpp
	$(CXX) $(CXXFLAGS) -o HostA HostA.cpp

Router: Router.cpp
	$(CXX) $(CXXFLAGS) -o Router Router.cpp

HostB: HostB.cpp
	$(CXX) $(CXXFLAGS) -o HostB HostB.cpp

.PHONY: clean run

clean:
	rm -f $(TARGETS)
