#Target  := raw_structure
Source  := src/main.cxx src/raw_decoder.cxx src/input_handler.cxx src/BenchMark.cxx src/GEMConfigure.cc src/prex_online_display.cxx
#SOURCE	:= $(patsubst %.cxx, ./src/%.cxx, $(Source) )
Target  := mpd4_decoder
#Source  := main.cc 
#Source  := etst2.cc

#Source	:= raw_structure_2.cxx
cc      := g++

libs    := $(shell root-config --libs)
glibs   := $(shell root-config --glibs)
cflags  := $(shell root-config --cflags)


incfile := -I${CODA}/Linux-x86_64/include

flags   := $(libs) $(glibs) $(cflags) $(incfile) -L${CODA}/Linux-x86_64/lib  -levioxx  -levio -lexpat

$(Target) : $(Source)
	@$(cc) -std=c++11 -O3 -o $(Target) $(Source) $(flags)
#	@$(cc)  -O3 -o $(Target) $(Source) $(flags)
clean:
	@rm $(Target)
