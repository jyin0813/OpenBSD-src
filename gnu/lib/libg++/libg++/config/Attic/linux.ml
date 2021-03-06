FULL_VERSION  = 27.1.0
MAJOR_VERSION = 27

SHLIB      = libg++.so.$(FULL_VERSION)
MSHLINK    = libg++.so.$(MAJOR_VERSION)
BUILD_LIBS = $(ARLIB) $(SHLIB) $(SHLINK) $(MSHLINK)
SHFLAGS    = -Wl,-soname,$(MSHLINK)
SHCURSES   = -lcurses
