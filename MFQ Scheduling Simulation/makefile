CC =g++
CXXFLAGS = -Wall -O2

SRCS=MFQ.c

TARGET = MFQ-sim

OBJS := $(patsubst %.cc,%.o,$(SRCS))


all: $(TARGET)

%.o:%.cc
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
.PHONY=clean
clean:
	rm -f $(OBJS) $(TARGET)












