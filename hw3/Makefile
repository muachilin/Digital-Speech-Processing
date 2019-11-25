SRIPATH ?=/home/mmlin/Desktop/srilm-1.5.10
MACHINE_TYPE ?=i686-m64
LM ?= bigram.lm

CXX=g++
CXXFLAGS=-O3 -I$(SRIPATH)/include -w --std=c++11

vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
RESULT_RUN = result2

TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map

LM_TRI ?= trigram.lm
TARGET_TRI = mydisambig_tri
SRC_TRI = mydisambig_tri.cpp
OBJ_TRI = $(SRC_TRI:.cpp=.o) 
RESULT_TRI = result3

.PHONY: all clean map run run_tri

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
run:
	if [ ! -d "$(RESULT_RUN)" ]; then \
		mkdir $(RESULT_RUN); \
	fi
	
	@
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt";\
		./mydisambig testdata/$$i.txt $(TO) $(LM) result2/$$i.txt;\
	    done;

run_tri: $(OBJ_TRI) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $(TARGET_TRI) $^
	
	if [ ! -d "$(RESULT_TRI)" ]; then \
		mkdir $(RESULT_TRI); \
	fi	
	
	@for i in $(shell seq 1 10) ; do \
		echo "Running trigram version $$i.txt";\
		./mydisambig_tri testdata/$$i.txt $(TO) $(LM_TRI) result3/$$i.txt;\
	    done; 


map: mapping
	@
	@echo "Mapping"
	@./mapping $(FROM) $(TO)

clean:
	$(RM) $(OBJ) $(TARGET) 
	$(RM) $(OBJ_TRI) $(TARGET_TRI)

