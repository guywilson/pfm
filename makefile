###############################################################################
#                                                                             #
# MAKEFILE for pfm                                                            #
#                                                                             #
# (c) Guy Wilson 2024                                                         #
#                                                                             #
###############################################################################

# Version number for pfm
MAJOR_VERSION = 1
MINOR_VERSION = 7

# Directories
SOURCE = src
BUILD = build
DEP = dep
DOC = doc

MANSOURCE = $(DOC)/pfm.1.md

# What is our target
TARGET = pfm
MANTARGET = $(TARGET).1

# Tools
VBUILD = vbuild
C = gcc
CPP = g++
LINKER = g++
PANDOC = pandoc

# postcompile step
PRECOMPILE = @ mkdir -p $(BUILD) $(DEP)
# postcompile step
POSTCOMPILE = @ mv -f $(DEP)/$*.Td $(DEP)/$*.d

#GLOBAL_DEF=-DPFM_TEST_SUITE_ENABLED
DEBUG_PASSWD=

CFLAGS_BASE=-c -Wall -pedantic -DSQLITE_HAS_CODEC $(GLOBAL_DEF)
CFLAGS_REL=$(CFLAGS_BASE) -O2
CFLAGS_DBG=$(CFLAGS_BASE) -g -DRUN_IN_DEBUGGER -DDEBUG_PASSWORD=\"$(DEBUG_PASSWD)\"

CPPFLAGS_BASE =-c -Wall -pedantic -std=c++20 -DSQLITE_HAS_CODEC $(GLOBAL_DEF)
CPPFLAGS_REL=$(CPPFLAGS_BASE) -O2
CPPFLAGS_DBG=$(CPPFLAGS_BASE) -g -DRUN_IN_DEBUGGER -DDEBUG_PASSWORD=\"$(DEBUG_PASSWD)\"

CPPFLAGS=$(CPPFLAGS_REL)
CFLAGS=$(CFLAGS_REL)
# CPPFLAGS=$(CPPFLAGS_DBG)
# CFLAGS=$(CFLAGS_DBG)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP)/$*.Td

PANDOCFLAGS = -s -t man

# Libraries
STDLIBS =
EXTLIBS = -lreadline -lhistory -lcurses -lsqlcipher -lgcrypt

COMPILE.cpp = $(CPP) $(CPPFLAGS) $(DEPFLAGS) -o $@
COMPILE.c = $(C) $(CFLAGS) $(DEPFLAGS) -o $@
LINK.o = $(LINKER) $(STDLIBS) -o $@

PANDOC.md = $(PANDOC) $(PANDOCFLAGS) -o $@

CSRCFILES = $(wildcard $(SOURCE)/*.c)
CPPSRCFILES = $(wildcard $(SOURCE)/*.cpp)
OBJFILES = $(patsubst $(SOURCE)/%.c, $(BUILD)/%.o, $(CSRCFILES)) $(patsubst $(SOURCE)/%.cpp, $(BUILD)/%.o, $(CPPSRCFILES))
DEPFILES = $(patsubst $(SOURCE)/%.c, $(DEP)/%.d, $(CSRCFILES)) $(patsubst $(SOURCE)/%.cpp, $(DEP)/%.d, $(CPPSRCFILES))

all: $(TARGET) $(MANTARGET)

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

$(MANTARGET): $(MANSOURCE)
	$(PANDOC.md) $<

.PRECIOUS = $(DEP)/%.d
$(DEP)/%.d: ;

-include $(DEPFILES)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin
	cp $(TARGET).1 /usr/local/share/man/man1

version:
	$(VBUILD) -incfile $(TARGET).ver -template version.c.template -out $(SOURCE)/version.c -major $(MAJOR_VERSION) -minor $(MINOR_VERSION)

clean:
	rm -r $(BUILD)
	rm -r $(DEP)
	rm $(TARGET)
	rm $(TARGET).1
