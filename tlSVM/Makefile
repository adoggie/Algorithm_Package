include ../Makefile.h

SOURCES		=CSVM.cpp  SVMThread.cpp
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlsvm.so 

.PHONY:         all clean
all:       first     $(MODULE)

first:
	mkdir -p $(FINAL_DIR)		


$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-shared -L$(FINAL_DIR) -ltlpublic
		
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC  -o $@ $< 

clean:
	rm -f $(OBJECTS) $(MODULE) 
