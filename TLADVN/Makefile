include ../Makefile.h

SOURCES		=AlgorithmBase.cpp      CurveReg.cpp        MultiAnova.cpp         Simuequation.cpp  SurvivalLT.cpp     TimeX12.cpp \
				Arima.cpp              Discriminant.cpp    PCR.cpp               RegressionOrdinal.cpp   Timer.cpp          TLADVN.cpp \
				Classify.cpp           ExponSmoothing.cpp  QuickCluster.cpp          SurvivalCox.cpp   TimeSeansonal.cpp  Var.cpp \
				CointegrationTest.cpp  Factor.cpp          RegLinearPredict.cpp        Survival.cpp      TimeSeries.cpp     VEC.cpp \
				Covariance.cpp         MarkovChain.cpp     RegressionLinear.cpp  RootTest.cpp            SurvivalKM.cpp    TimeX11.cpp \
				RidgeAnalysize.cpp RegressionProbit.cpp RegressionLogistic.cpp

OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtladvn.so 

.PHONY:         all clean
all:       first     $(MODULE)

first:
	mkdir -p $(FINAL_DIR)		


$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-shared -L$(FINAL_DIR) -ltlresult -ltlcpknl
		
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC  -o $@ $< 

clean:
	rm -f $(OBJECTS) $(MODULE) 