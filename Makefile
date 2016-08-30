# Third-party libraries
MATLABROOT  ?= D:\Progra~1\MATLAB\R2010a
MEXLIBS     ?= -llibmx -llibmex -llibut

CC           = g++
CCFLAGS      = 
CCFLAGS     += -I"${MATLABROOT}/extern/include" -L"${MATLABROOT}/extern/lib/win32/microsoft"
MEXEXT      ?= $(shell mexext)

target      ?= mexgetopt

all: $(target).$(MEXEXT)

$(target).$(MEXEXT): mexgetopt.c
	$(CC) $(CCFLAGS) -shared -o $@ $^ ${MEXLIBS}

format:
	@astyle style=kr indent=spaces=2 -p -U --recursive --suffix=none "*.hpp" "*.h" "*.cpp" "*.c"

clean:
	@-rm *.o *.mexw32
