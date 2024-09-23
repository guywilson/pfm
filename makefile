###############################################################################
#                                                                             #
# MAKEFILE for pfm                                                            #
#                                                                             #
# (c) Guy Wilson 2024                                                         #
#                                                                             #
###############################################################################

# Version number for pfm
MAJOR_VERSION = 0
MINOR_VERSION = 4

# Directories
SOURCE = src
BUILD = build
DEP = dep

# What is our target
TARGET = pfm

# Tools
VBUILD = vbuild
C = gcc
CPP = g++
LINKER = g++

# postcompile step
PRECOMPILE = @ mkdir -p $(BUILD) $(DEP)
# postcompile step
POSTCOMPILE = @ mv -f $(DEP)/$*.Td $(DEP)/$*.d

#GLOBAL_DEF=-DPFM_TEST_SUITE_ENABLED

CFLAGS_BASE=-c -Wall -pedantic -DSQLITE_HAS_CODEC $(GLOBAL_DEF)
CFLAGS_REL=$(CFLAGS_BASE) -O2
CFLAGS_DBG=$(CFLAGS_BASE) -g

CPPFLAGS_BASE = -c -Wall -pedantic -std=c++20 -DSQLITE_HAS_CODEC $(GLOBAL_DEF)
CPPFLAGS_REL=$(CPPFLAGS_BASE) -O2
CPPFLAGS_DBG=$(CPPFLAGS_BASE) -g

CPPFLAGS=$(CPPFLAGS_REL)
CFLAGS=$(CFLAGS_REL)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP)/$*.Td

# Libraries
STDLIBS = 
EXTLIBS = -lreadline -lsqlcipher

COMPILE.cpp = $(CPP) $(CPPFLAGS) $(DEPFLAGS) -o $@
COMPILE.c = $(C) $(CFLAGS) $(DEPFLAGS) -o $@
LINK.o = $(LINKER) $(STDLIBS) -o $@

CSRCFILES = $(wildcard $(SOURCE)/*.c)
CPPSRCFILES = $(wildcard $(SOURCE)/*.cpp)
OBJFILES = $(patsubst $(SOURCE)/%.c, $(BUILD)/%.o, $(CSRCFILES)) $(patsubst $(SOURCE)/%.cpp, $(BUILD)/%.o, $(CPPSRCFILES))
DEPFILES = $(patsubst $(SOURCE)/%.c, $(DEP)/%.d, $(CSRCFILES)) $(patsubst $(SOURCE)/%.cpp, $(DEP)/%.d, $(CPPSRCFILES))

all: $(TARGET)

# Compile C/C++ source files
#
$(TARGET): $(OBJFILES)
	$(LINK.o) $^ $(EXTLIBS)

$(BUILD)/%.o: $(SOURCE)/%.c
$(BUILD)/%.o: $(SOURCE)/%.c $(DEP)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(BUILD)/%.o: $(SOURCE)/%.cpp
$(BUILD)/%.o: $(SOURCE)/%.cpp $(DEP)/%.d
	$(PRECOMPILE)
	$(COMPILE.cpp) $<
	$(POSTCOMPILE)

.PRECIOUS = $(DEP)/%.d
$(DEP)/%.d: ;

-include $(DEPFILES)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin

version:
	$(VBUILD) -incfile $(TARGET).ver -template version.c.template -out $(SOURCE)/version.c -major $(MAJOR_VERSION) -minor $(MINOR_VERSION)

clean:
	rm -r $(BUILD)
	rm -r $(DEP)
	rm $(TARGET)
