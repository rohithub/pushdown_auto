compileAndRun:
	@echo "Name of module : "; \
	read module; \
	g++ -o lr0.o $$module.cpp;
	./lr0.o

compileLR0:
	-g++ -o lr0.o LR0.cpp
clean:
	rm *.o

.PHONY: compile
