# ------------------------------------------------
# Generic Makefile
#
# Author: benyamin@israelmail.com
# Date  : 
# 
# ------------------------------------------------

# project name 
TARGET   = cxn_cc

CC       = g++
# compiling flags
CFLAGS   =  -Wall -I.

LINKER   = g++ -o
# linking flags
LFLAGS   = -Wall -I. -lm

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
    @$(LINKER) $@ $(LFLAGS) $(OBJECTS)
    @echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
    @$(CC) $(CFLAGS) -c $< -o $@
    @echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
    @$(rm) $(OBJECTS)
    @echo "Cleanup complete!"

.PHONY: remove
remove: clean
    @$(rm) $(BINDIR)/$(TARGET)
    @echo "Executable removed!"
<<<<<<< HEAD

=======
>>>>>>> 8f46cee88a30e3c795e2284f15106e116641f81a
