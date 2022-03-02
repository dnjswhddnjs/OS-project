CC =g++
CXXFLAGS = -Wall -O2

SRCS=OS41_2021-2_2015310464_원종원_P2.c.c

TARGET = VMM-sim

OBJS := $(patsubst %.cc,%.o,$(SRCS))


all: $(TARGET)

%.o:%.cc
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
.PHONY=clean
clean:
	rm -f $(OBJS) $(TARGET)












