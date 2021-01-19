##############################################
#	MAKEFILE	
#	author: Siyu Jian 
#	emal:   jiansiyu@gmail.com
#
#		Physics Department of UNiversity of Virginia
#		Charlottesville, VA, 22903
#		United States
##############################################



#+++++++++++++++++++++++++++++++++++++++++++
# general make file configuration

CC       = g++ -std=c++11 -pthread -lpthread -Ofast -g3 #-Wall
CFLAGS= ${CFLAG}
THIS_DIR =`cd "\`dirname \"$0\"\`";pwd`

#------------------------------------------------------------------------------
# ROOT related configuration
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLDFLAGS  := $(shell root-config --ldflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTHASTHREAD    := $(shell root-config --has-thread)
ROOTAUXLIB       := $(shell root-config --auxlibs)
ROOTAUXCFLAG     := $(shell root-config --auxcflags)
#------------------------------------------------------------------------------


CXXFLAGS  +=${ROOTCFLAGS} ${CFLAGS} -I${EVIO_INC} -I./ 
LDFLAGS	  +=${ROOTLDFLAGS}

ifeq ($(shell uname -s), Linux)
LIBS      +=${ROOTLIBS} ${ROOTGLIBS} -lMinuit -L${EVIO_LIB}  -lexpat -levioxx -levio  -lstdc++ -lm
else
LIBS      +=${ROOTLIBS} ${ROOTGLIBS} -lMinuit -L${EVIO_LIB}  -lexpat ${EVIO_LIB}/libevioxx.dylib ${EVIO_LIB}/libevio.dylib 
endif

GLIBS     +=${ROOTGLIBS} ${SYSLIBS}
LINKOPTION += -pthread -lm -ldl -lconfig++


# searching for the link files and its source file
SLOT_SOURCE +=${patsubst %LinkDef.h, %.cpp,  ${wildcard ./*LinkDef.h}}
EXCLUDE_SOURCE +=     #${SLOT_SOURCE} 
ALLSOURCE += ${wildcard ./*.cpp}
ALLSOURCE += ${wildcard ./*.C}
ALLSOURCE += ${wildcard ./*.cxx}
SOURCE =${ALLSOURCE} #${filter-out ${EXCLUDE_SOURCE},${ALLSOURCE}}

SLOT_TARGET += ${patsubst %.cpp, %Dic.cxx, ${SLOT_SOURCE}}
SLOT_OBJS   += ${patsubst ./%, ./bin/%, ${patsubst %.cpp, %Dic.o, ${SLOT_SOURCE}}}
SOURCE_OBJS += ${patsubst ./%, ./bin/%, ${addsuffix .o, ${basename ${SOURCE}}}}
OBJS    +=  ${SOURCE_OBJS} ${SLOT_OBJS}

TARGET = runVerifier
all: ${TARGET}  

${TARGET}: ${OBJS} 
	@echo ${THIS_DIR}
	@echo ${OBJS}
	@echo 'Building target: $@'
	@mkdir -p $(@D)
	@$(CC)  $(OBJS)  $(LIBS) ${LIBS}  ${LINKOPTION} -o  $@
	#@cp ${patsubst %.cpp, %Dic_rdict.pcm, ${SLOT_SOURCE}} ${THIS_DIR}/
	@echo 'Finish building: $@'
	#@{RM} ${OBJS}
	@echo

./bin/%.o : %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@mkdir -p $(@D)
	@$(CC)  ${CXXFLAGS} -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finish building: $<'
	@echo
	
./bin/%.o : %.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@mkdir -p $(@D)
	@$(CC)  ${CXXFLAGS} -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finish building: $<'
	@echo
	
bin/%.o : %.C
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@mkdir -p $(@D)
	@$(CC)  ${CXXFLAGS} -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finish building: $<'
	@echo
	
./%Dic.cxx : ./%.h ./%LinkDef.h
	@echo 'Building file: $@'
	@echo 'Invoking: rootcling Compiler'
	@mkdir -p $(@D)
	@rootcint -f "$@" -c  ${patsubst %Dic.cxx, %.h, $@} ${patsubst %Dic.cxx, %LinkDef.h, $@}
	
./bin/%Dic.o : ./%Dic.cxx 
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@mkdir -p $(@D)
	@$(CC)  ${CXXFLAGS} -I./ -c  $^ -o $@
	@echo 'Finish building: $<'
	@echo

#	
JUNK_L1 = *~ ./*~ ./*/*~
JUNK_L2 = ${OBJS}
JUNK_L3 = *_rdict.pcm ./*_rdict.pcm ./*/*_rdict.pcm  ${TARGET}
	 
PHONY: clean help test

clean:
	${RM} ${OBJS} ${TARGET} *~ ./*/*Dic.cxx ./*/*rdict.pcm *rdict.pcm ./*/*~
	${RM} ./bin/* -r
help:
	cat MicroDefinatonList.df

test: 
	@echo "Build Target"
	@echo ${TARGET}
	@echo
	@echo "slot source "
	@echo ${SLOT_SOURCE}
	@echo "slot Dic"
	@echo  ${SLOT_TARGET}
	@echo 
	@echo "Source"
	@echo ${SOURCE}
