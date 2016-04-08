include ../Makefile.h

SOURCES		= 	BPArithmetic.cpp  BP.cpp  BPNet.cpp  BPNetInterface.cpp  \
					BPNeuralVO.cpp  Matrix.cpp  MemoStream.cpp  tlBP.cpp

OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlbp.so

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
