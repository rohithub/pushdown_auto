compile:
	@echo "Name of module : "; \
	read module; \
	g++ -o pda.o $$module.cpp;
	./pda.o

clean:
	rm *.o

.PHONY: compile
