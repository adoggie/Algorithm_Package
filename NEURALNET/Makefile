include ../Makefile.h

SOURCES		= 	ARTClusterLayer.cpp ARTInterfaceNode.cpp Bedlam.cpp KLayer.cpp NeuralNet.cpp \
				RBTopology.cpp ARTClusterNode.cpp ARTNode.cpp BPTopology.cpp KNode.cpp \
				NeuralNetDLL.cpp Sample.cpp Topology.cpp ARTInterfaceLayer.cpp  \
				ARTTopology.cpp HopGen.cpp KohonenTopology.cpp NeuralNetInterface.cpp SampleSet.cpp

OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlneuralnet.so

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
