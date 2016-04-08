include ../Makefile.h

SOURCES		= DecTreeCol.cpp  DecTreeCtrl.cpp    Evaluate.cpp DecTree.cpp \
				DecTreeThread.cpp     tlDecTree.cpp

OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlnewdectree.so 

.PHONY:         all clean
all:       first     $(MODULE)

first:
	mkdir -p $(FINAL_DIR)		


$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-shared -L$(FINAL_DIR) 
		
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC  -o $@ $< 

clean:
	rm -f $(OBJECTS) $(MODULE) 
