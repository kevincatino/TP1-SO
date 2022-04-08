SHELL:=/bin/bash
GCC=gcc
GCC_FLAGS=-Wall -pthread
GCC_DEBUG_FLAGS=-g

OBJECT_APP := ./app.c
OBJECT_SLAVE := ./slave.c
OBJECT_VIEW := ./view.c
OBJECT_UTILITIES := ./utilities.c
OBJECT_MASTER_ADT := ./masterADT.c
OBJECT_SH_MEM_ADT := ./sh_mem_ADT.c