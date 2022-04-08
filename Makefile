include Makefile.inc

all: src
	@echo -e "\e[1;3;36m[Project compiled] \e[0m"

src:
	@cd src; make all

clean:
	@cd src; make clean
	@echo -e '\e[1;3;35m[Project cleaned] \e[0m'


.PHONY: src all clean