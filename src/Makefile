include ../Makefile.inc

SOURCES := $(wildcard *.c)
OBJECTS := $(wildcard *.o)
TARGETS := $(wildcard ../*.out)

OBJECTS_APP := $(OBJECT_APP:.c=.o) $(OBJECT_MASTER_ADT:.c=.o) $(OBJECT_SH_MEM_ADT:.c=.o) $(OBJECT_UTILITIES:.c=.o)
OBJECTS_SLAVE := $(OBJECT_SLAVE:.c=.o) $(OBJECT_UTILITIES:.c=.o)
OBJECTS_VIEW := $(OBJECT_VIEW:.c=.o) $(OBJECT_SH_MEM_ADT:.c=.o) $(OBJECT_UTILITIES:.c=.o)

# Asi se van a llamar los .out
TARGET_APP := app
TARGET_SLAVE := slave
TARGET_VIEW := view

all: $(TARGET_APP) $(TARGET_SLAVE) $(TARGET_VIEW)
	@echo "\033[1;3;36m[src compiled]\033[0m"


$(TARGET_APP): $(OBJECTS_APP)
	$(GCC) $(GCC_FLAGS) $(OBJECTS_APP) -o ../$(TARGET_APP).out
	@echo "\033[3;36m[Solver compiled]\033[0m"

$(TARGET_SLAVE): $(OBJECTS_SLAVE)
	$(GCC) $(GCC_FLAGS) $(OBJECTS_SLAVE) -o ../$(TARGET_SLAVE).out
	@echo "\033[3;36m[Slave compiled]\033[0m"

$(TARGET_VIEW): $(OBJECTS_VIEW)
	$(GCC) $(GCC_FLAGS) $(OBJECTS_VIEW) -o ../$(TARGET_VIEW).out
	@echo "\033[3;36m[View compiled]\033[0m"



%.o: %.c
	$(GCC) $(GCC_FLAGS) -I./include -c $< -o $@
	@echo "\033[32mCompiled object file \033[0m $@"


clean:
	rm -rf $(OBJECTS) $(TARGETS)
	@echo "\033[3;35m[src cleaned]\033[0m"
	
.PHONY: all clean $(TARGET_APP) $(TARGET_SLAVE) $(TARGET_VIEW)