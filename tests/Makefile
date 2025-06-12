# Simple Makefile for tests, replace in the future?

CXXFLAGS    := -std=c++23 -I../dynadjust -I../dynadjust/include
LDFLAGS 	:= 

# On macOS, link against Accelerate framework
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework Accelerate
endif

all: test_matrix test_dna_io_bst

test_matrix: test_matrix.cpp ../dynadjust/include/math/dnamatrix_contiguous.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test_dna_io_bst: test_dna_io_bst.cpp ../dynadjust/include/io/dnaiobst.cpp ../dynadjust/include/io/dnaiobase.cpp ../dynadjust/include/measurement_types/dnastation.cpp
	$(CXX) $(CXXFLAGS) -D__BINARY_NAME__='"test_dna_io_bst"' -D__BINARY_DESC__='"Unit tests for dna_io_bst class"' $^ -o $@ $(LDFLAGS)

clean:
	rm -fr test_matrix test_dna_io_bst
