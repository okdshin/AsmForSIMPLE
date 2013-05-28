CXX = g++ -std=gnu++11
CXXFLAGS = -Wall -g -D EXCEPTION_UNIT_TEST
INCLUDES = 
LIBS =
OBJS = Exception.o
PROGRAM = Exception.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
