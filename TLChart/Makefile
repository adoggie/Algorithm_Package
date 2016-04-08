include ../Makefile.h

SOURCES		= 	Area.cpp  Box.cpp    Correlation.cpp       Emp.cpp        Line.cpp  Scatter.cpp          Statistics.cpp  tlChart.cpp \
					Bar.cpp   Chart.cpp  CrossCorrelation.cpp  Histogram.cpp  Pie.cpp   SelfCorrelation.cpp 
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlchart.so

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
