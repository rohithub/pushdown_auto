compile:
	@echo "Name of module : "; \
	read module; \
	g++ -o lr0.o $$module.cpp;
	./lr0.o

clean:
	rm *.o

.PHONY: compile
