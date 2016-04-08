include ../Makefile.h

SOURCES		= 	BlurryCluster.cpp  BlurryInterface.cpp  TLBLURRYCLUSTER.cpp
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlblurrycluster.so

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
