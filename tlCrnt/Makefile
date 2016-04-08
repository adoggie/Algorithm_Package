include ../Makefile.h

SOURCES= AAnova.cpp        ChiSquare.cpp    crosstab.cpp  OneSmplTTest.cpp  \
				RunTest.cpp      Statistics.cpp  TTest.cpp Analyze.cpp CKTest.cpp DesLayerConf.cpp  \
				PairTTest.cpp     Script.cpp       \
				BinomialTest.cpp  Correlation.cpp  NonParamTest.cpp  PartialCorre.cpp  StatiMethod.cpp  tlCrnt.cpp

OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlcrnt.so 

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
