include ../Makefile.h

SOURCES		= 	Association.cpp  Common.cpp               MultiDimRule.cpp    Section.cpp \
					AssocRule.cpp    FileAssociationRule.cpp  MultiLayerRule.cpp  SingleRuleReduction.cpp  tlDtmn.cpp

OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtldtmn.so

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
