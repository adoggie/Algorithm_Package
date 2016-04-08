include ../Makefile.h

SOURCES		= Manager.cpp  TCurSys.cpp  algsvc.cpp #tlBase.cpp
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/algsvc

.PHONY:         all clean
all:       first     $(MODULE)

first:
	mkdir -p $(FINAL_DIR)		


$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-L$(FINAL_DIR) -ltlpublic -ltlcpknl -ltldbif -ltlresult -lpthread -ldl -ltlarithif
		
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS)  -o $@ $< 

clean:
	rm -f $(OBJECTS) $(MODULE) 
