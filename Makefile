exe = ceci 

cc = "$(shell which g++)" 

flags = -I -std=c++17

ifeq ($(debug), 1)
	flags+= -O0 -g #-DDEBUG 
else
	flags += -O
endif

objs = $(patsubst %.cpp,%.o,$(wildcard ../../lib/*.cpp)) \
			$(patsubst %.cpp,%.o,$(wildcard *.cpp))

deps = $(wildcard ./*.cpp) \
				$(wildcard *.h) \
				Makefile

%.o:%.cpp $(deps)
	$(cc) -c $< -o $@ $(flags)

$(exe):$(objs)
	$(cc) $(objs) -o $(exe) $(flags)

clean:
	rm -rf $(exe) $(objs) 
