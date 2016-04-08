include ../Makefile.h

SOURCES		= 	AnaSyntax.cpp FileReadWrite.cpp System.cpp \
				Util.cpp AnaWord.cpp Packet.cpp TCLFile.cpp TString.cpp
OBJECTS		= $(SOURCES:.cpp=.o)
MODULE		= $(FINAL_DIR)/libtlpublic.so

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
