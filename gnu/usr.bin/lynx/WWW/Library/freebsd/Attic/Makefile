#  Make WWW under FreeBSD
#

# For W3 distribution, machine type for subdirectories
WWW_MACH = freebsd

# The ASIS repository's name for the machine we are on
ASIS_MACH = i386/FreeBSD


CFLAGS += -DDEBUG
LFLAGS =

# Directory for installed binary:
BINDIR = /usr/local/bin

# Where is the W3 object library to be installed (not normally done)?
LIBDIR = $(WWW)/Library/Implementation/$(WWW_MACH)

#_________________ OK if normal W3 distribution
# Where is the WWW source root?
WWW = ../..

#  Where should temporary (object) files go?
WTMP = ../..

include $(WWW)/Library/Implementation/CommonMakefile
