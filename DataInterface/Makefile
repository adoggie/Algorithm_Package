include ../Makefile.h

SOURCES		=DataInterface.cpp TCDataArray.cpp
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtldbif.so 

.PHONY:         all clean
all:       first     $(MODULE)

first:
	@mkdir -p $(FINAL_DIR)		


$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-shared -L$(FINAL_DIR) -ltldbdll -ltlpublic
		
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC  -o $@ $< 

clean:
	rm -f $(OBJECTS) $(MODULE) 
