compile:
	@echo "Name of module : "; \
	read module; \
	g++ -o $$module.o $$module.cpp

clean:
	rm *.o
