# Складання без CLion: make && make test
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2
SRCS     = $(wildcard src/*.cpp)
OBJS     = $(SRCS:.cpp=.o)

wish: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

src/%.o: src/%.cpp src/shell.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: wish
	./test-wish.sh -c

clean:
	rm -f $(OBJS) wish
	rm -rf tests-out

.PHONY: test clean
