include ./Makefile.inc

all:  src
	@echo "\033[1;3;36m[Proyect compiled]\033[0m"

src:
	@cd src; make all

clean:
	@cd src; make clean
	@echo "\033[1;3;35m[Proyect cleaned]\033[0m"


.PHONY: src all clean