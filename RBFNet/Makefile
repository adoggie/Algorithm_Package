include ../Makefile.h

SOURCES		= 	KMeans_Algorithm.cpp  RBFN_Algorithm.cpp  RBFNet.cpp
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlrbfnet.so

.PHONY:  all clean

all:    first  $(MODULE)

first:
	@mkdir -p $(FINAL_DIR)	
	
	
$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-shared

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC  -o $@ $<

clean:
	rm -f $(OBJECTS) $(MODULE)
