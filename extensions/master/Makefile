# Makefile for the hash library

include ../MakefileCommons

# C source files
csrcs = master.c mqhelper.c 
# header files
header = zhelpers.h queue.h
# directory where the binaries should be copied to
prob_libdir = ../../lib

# the target file
basename=master
shlib=$(basename).$(shext)
targetlib = $(prob_libdir)/$(shlib)

# Declaration of which targets are no files, just "phony" targets
.PHONY: install ltlc Linux Darwin clean

install: $(targetlib) 
$(targetlib): $(shlib)	
	cp -f $(shlib) $(targetlib)


# default target: we build for the current system 
$(basename): $(system)

Linux: $(basename).so $(send)
Darwin:$(basename).bundle $(send)
Windows: $(basename).dll $(send).exe

$(basename).so $(basename).bundle: $(csrcs) $(header) $(basename).pl
	$(splfr) --cflag="-Wall" --resource=$(basename) --source=$(basename).pl  $(csrcs) -lzmq -lczmq

clean:
	rm -f *.o *.so *.bundle *.dll *.obj  \
		*_glue*.*  *.sav


###########################################################################
# cross-compilation of Windows-DLLs in Linux with mingw
# - you must set the SICSTUSDIR environment variable to use this
# - you must install mingw

dllobjects = $(csrcs:.c=.obj) $(basename)_glue.obj

$(basename).dll: $(dllobjects) $(basename)_glue.mapfile 
	$(checksicstusdir)
	$(MINGWCC) --shared --dll -Wl,--version-script=$(basename)_glue.mapfile -lm $(dllobjects) -o $(basename).dll

$(basename)_glue.c $(basename)_glue.h $(basename)_glue.mapfile: $(basename).pl
	$(splfr) --nocompile --resource=$(basename) --source=$(basename).pl 

$(dllobjects): %.obj: %.c
	$(checksicstusdir)
	$(MINGWCC) -DSP_RESNAME=$(basename) $(LTLFLAGS) $(SICSTUSCFLAGS) -c $(@:.obj=.c) -o $(@)
