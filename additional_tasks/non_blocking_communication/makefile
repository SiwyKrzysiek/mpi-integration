ProgramName=pizzaStore
MPICC?=mpicc

DebugFlags=-std=c11 -Wall -g3 -pedantic
ReleseFlags=-Ofast -std=c11 -g0 -Wno-everything

Objects=

release: CompileFlags=$(ReleseFlags)
release: $(Objects)
	$(MPICC) $(CompileFlags) -o $(ProgramName) $(Objects) main.c

rebuild: clean release

debug: CompileFlags=$(DebugFlags) -D DEBUG
debug: $(Objects)
	$(MPICC) $(CompileFlags) -o $(ProgramName) $(Objects) main.c

clean:
	rm -rf *.dSYM
	rm -f *.o

%.o : %.c %.h
	$(MPICC) $(CompileFlags) -c $< -o $@
