# Simple Makefile for tests, replace in the future?

CXXFLAGS    := -std=c++23 -I../dynadjust -I../dynadjust/include
LDFLAGS 	:= 

# On macOS, link against Accelerate framework
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework Accelerate
endif

all: test_matrix

test_matrix: test_matrix.cpp ../dynadjust/include/math/dnamatrix_contiguous.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -fr test_matrix
