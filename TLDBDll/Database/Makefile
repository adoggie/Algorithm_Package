include ../../Makefile.h

SOURCES		=COriginRecPos.cpp DataAccess.cpp DriverBase.cpp DriverManager.cpp\
				Missing.cpp PageArraySort.cpp cbooleanfield.cpp ccurrencyfield.cpp\
				cdataset.cpp cdatefield.cpp cdoublefield.cpp cfakerecno.cpp\
				cfakerecnos.cpp cfield.cpp cfields.cpp cfileoperation.cpp\
				cheader.cpp cintfield.cpp cpage.cpp cpages.cpp cqueueitem.cpp cstringfield.cpp\
				cvaluelabel.cpp fieldlist.cpp fieldsversion.cpp showcharacteristic.cpp tempfile.cpp versionlist.cpp
				
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtldbdll.so


.PHONY:  all clean

all:    first  $(MODULE)

first:
	@mkdir -p $(FINAL_DIR)	
	
	
$(MODULE):  $(OBJECTS)	
	$(CXX) -o $@ $(OBJECTS)	-shared -L$(FINAL_DIR) -ltlpublic

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC  -o $@ $<

clean:
	rm -f $(OBJECTS) $(MODULE)
