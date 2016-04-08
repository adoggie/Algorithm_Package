include Makefile.h
#export 		CFLAGS CXXFLAGS

SUBDIRS		= 	Public cpknl DataDeal DataInterface  NEURALNET NewDecTree RBFNet \
				Result TLADVN tlBase TLBLURRYCLUSTER tlBP TLChart tlCrnt TLDBDll \
				tlDtmn tlNested TLROUGH tlSVM
				 
 
.PHONY:		all clean $(SUBDIRS) list
all:		$(SUBDIRS)


Public:
	cd $@; $(MAKE)
cpknl:Public
	cd $@; $(MAKE)
DataDeal:Public DataInterface
	cd $@; $(MAKE)
DataInterface:Public
	cd $@; $(MAKE)
NEURALNET:Public
	cd $@; $(MAKE)
NewDecTree:Public
	cd $@; $(MAKE)
RBFNet:Public
	cd $@; $(MAKE)
Result:Public
	cd $@; $(MAKE)
TLADVN:Public Result
	cd $@; $(MAKE)
tlBase:Public cpknl DataDeal DataInterface  NEURALNET NewDecTree RBFNet \
				Result TLADVN tlBase TLBLURRYCLUSTER tlBP TLChart tlCrnt TLDBDll \
				tlDtmn tlNested TLROUGH tlSVM
	cd $@; $(MAKE)
TLBLURRYCLUSTER:Public
	cd $@; $(MAKE)
tlBP:Public
	cd $@; $(MAKE)
TLChart:Public
	cd $@; $(MAKE)
tlCrnt:Public
	cd $@; $(MAKE)
TLDBDll:Public
	cd $@/Database; $(MAKE)
tlDtmn:Public
	cd $@; $(MAKE)
tlNested:Public
	cd $@; $(MAKE)
TLROUGH:Public
	cd $@; $(MAKE)
tlSVM:Public
	cd $@; $(MAKE)

clean:
	for subdir in $(SUBDIRS); do \
	  (cd $${subdir}; $(MAKE) $@); \
	done

list:
	@echo 'dbif     --  dbinterface (libtldbif.so)'
	@echo 'dbdll    --  tldbdll(libtldbdll.so)'
	@echo 'cpknl    --  tlcpknl(libcpknl.so)'
	@echo 'public   --  tlpublic(libtlpublic.so)'
	@echo 'result   --  tlresult(libtlresult.so)'
	@echo 'base     --  tlbase(libtlbase.so)'
	@echo 'arithif  --  (libtlarithif.so)
