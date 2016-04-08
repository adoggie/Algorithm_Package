include ../Makefile.h

SOURCES		=	CResult.cpp TChartArea.cpp TChartControl.cpp TChartHistogram.cpp TChartROC.cpp \
				DecTreeWnd.cpp TChartAutocorrelations.cpp TChartCrossCorrelations.cpp TChartLine.cpp \
				TChartScatterPlot.cpp Output.cpp TChartBar.cpp TChartCurve.cpp TChartPareto.cpp TChartSequence.cpp \
				Result.cpp TChartBlurry.cpp TChartDecTree.cpp TChartPedigree.cpp TGraphData.cpp \
				TChartBoxplot.cpp TChartErrBar.cpp TChartPie.cpp \
				TChart1.cpp TChartClustering.cpp TChartHighLow.cpp TChartPP.cpp

OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlresult.so 

.PHONY:         all clean
all:       first     $(MODULE)

first:
	@mkdir -p $(FINAL_DIR)		


$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-shared -lm -ltlcpknl -ltlpublic -L$(FINAL_DIR)
		
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC  -o $@ $< -D_DEBUG

clean:
	rm -f $(OBJECTS) $(MODULE) 