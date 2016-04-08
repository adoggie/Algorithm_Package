include ../Makefile.h

SOURCES		=Complex.cpp cpknl.cpp dbdsqr.cpp dcopy.cpp dgebd2.cpp dgebrd.cpp dgelq2.cpp dgelqf.cpp dgemm.cpp dgemv.cpp dgeqp3.cpp dgeqr2.cpp dgeqrf.cpp dger.cpp dgesvd.cpp dlabrd.cpp dlacpy.cpp dlamch.cpp dlange.cpp dlapy2.cpp dlaqp2.cpp dlaqps.cpp dlarf.cpp dlarfb.cpp dlarfg.cpp dlarft.cpp dlartg.cpp dlas2.cpp dlascl.cpp dlaset.cpp dlasq1.cpp dlasq2.cpp dlasq3.cpp dlasq4.cpp dlasq5.cpp dlasq6.cpp dlasr.cpp dlasrt.cpp dlassq.cpp dlasv2.cpp dnrm2.cpp dorg2r.cpp dorgbr.cpp dorgl2.cpp dorglq.cpp dorgqr.cpp dorm2r.cpp dormbr.cpp dorml2.cpp dormlq.cpp dormqr.cpp drot.cpp dscal.cpp dswap.cpp dtrmm.cpp dtrmv.cpp d_sign.cpp ErrorReporter.cpp exit_.cpp f77_aloc.cpp idamax.cpp ieeeck.cpp ilaenv.cpp i_dnnt.cpp lsame.cpp Matrix.cpp pow_dd.cpp pow_di.cpp qr_main.cpp RandFactory.cpp sig_die.cpp  s_cat.cpp s_cmp.cpp s_copy.cpp Timer.cpp Vector.cpp xerbla.cpp 				
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlcpknl.so 

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
