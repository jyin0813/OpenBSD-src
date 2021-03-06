# Generated automatically from Makefile.in by configure.
#
# $KTH: Makefile.in,v 1.53.2.2 2000/10/19 20:27:21 assar Exp $
#

SHELL = /bin/sh

srcdir = .

CC = gcc
LINK = $(CC)
AR = ar
RANLIB = ranlib
LN_S = ln -s
DEFS = -DHAVE_CONFIG_H
CFLAGS =  -g -O2 $(WFLAGS)
WFLAGS = 
LD_FLAGS = 
LDSHARED = ld -Bshareable

INSTALL = /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
MKINSTALLDIRS = ../../mkinstalldirs

prefix = /usr/athena
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
transform=s,x,x,
EXECSUFFIX=

PICFLAGS = 
 
LIB_DEPS =  -lc
build_symlink_command   = @true
install_symlink_command = @true

PROGS = destest$(EXECSUFFIX) \
	mdtest$(EXECSUFFIX) \
	md5crypt_test$(EXECSUFFIX) \
	des$(EXECSUFFIX) \
	rpw$(EXECSUFFIX) \
	speed$(EXECSUFFIX)
PROGS2INSTALL = des$(EXECSUFFIX)
LIBNAME = $(LIBPREFIX)des
LIBEXT = a
LIBPREFIX = lib
SHLIBEXT = nobuild
LIB = $(LIBNAME).$(LIBEXT)

# Generated with lorder *.o | tsort | xargs echo

LIBSRC = \
	xcbc_enc.c sha.c rnd_keys.c rc4_skey.c rc4_enc.c qud_cksm.c	      \
	ofb_enc.c ofb64enc.c ofb64ede.c ncbc_enc.c md4.c key_par.c fcrypt.c   \
	enc_writ.c enc_read.c ede_enc.c des.c cfb_enc.c cfb64enc.c cfb64ede.c \
	cbc3_enc.c read_pwd.c pcbc_enc.c md5.c ecb_enc.c ecb3_enc.c cbc_enc.c \
	str2key.c set_key.c cbc_cksm.c des_enc.c

LIBOBJ = \
	xcbc_enc.o sha.o rnd_keys.o rc4_skey.o rc4_enc.o qud_cksm.o	      \
	ofb_enc.o ofb64enc.o ofb64ede.o ncbc_enc.o md4.o key_par.o fcrypt.o   \
	enc_writ.o enc_read.o ede_enc.o des.o cfb_enc.o cfb64enc.o cfb64ede.o \
	cbc3_enc.o read_pwd.o pcbc_enc.o md5.o ecb_enc.o ecb3_enc.o cbc_enc.o \
	str2key.o set_key.o cbc_cksm.o des_enc.o

all: $(LIB) $(PROGS)

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

COM = $(CC) -c $(CPPFLAGS) $(DEFS) -I../../include -I$(srcdir) $(PICFLAGS)

.c.o:
	$(COM) $(CFLAGS) $<

# Compile this file without debug if using gcc
des_enc.o: des_enc.c
	@if test "$(CC)" = gcc; then\
	  echo "$(COM) $(CFLAGS) -fomit-frame-pointer -O3 $(srcdir)/des_enc.c"; \
	  $(COM) $(CFLAGS) -fomit-frame-pointer -O3 $(srcdir)/des_enc.c; \
	else \
	  echo "$(COM) $(CFLAGS) $(srcdir)/des_enc.c"; \
	  $(COM) $(CFLAGS) $(srcdir)/des_enc.c; \
	fi

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(libdir)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(libdir)/$(LIB)
	@true
	$(MKINSTALLDIRS) $(DESTDIR)$(bindir)
	for x in $(PROGS2INSTALL); do \
	  $(INSTALL_PROGRAM) $$x $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done

uninstall:
	rm -f $(DESTDIR)$(libdir)/$(LIB)
	for x in $(PROGS2INSTALL); do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done

TAGS: $(LIBSRC)
	etags $(LIBSRC)

check: destest$(EXECSUFFIX) mdtest$(EXECSUFFIX) md5crypt_test$(EXECSUFFIX)
	./destest$(EXECSUFFIX)
	./mdtest$(EXECSUFFIX)
	./md5crypt_test$(EXECSUFFIX)

clean:
	rm -f $(LIB) *.so *.so.* so_locations *.o *.a $(PROGS)

mostlyclean: clean

distclean: clean
	rm -f Makefile *.tab.c *~

realclean: distclean
	rm -f TAGS

$(LIBNAME)_pic.a: $(LIBOBJ)
	rm -f $@
	$(AR) cr $@ $(LIBOBJ)
	-$(RANLIB) $@

$(LIBNAME).a: $(LIBOBJ)
	rm -f $@
	$(AR) cr $@ $(LIBOBJ)
	-$(RANLIB) $@

$(LIBNAME).$(SHLIBEXT): $(LIBOBJ) $(LIBNAME)_pic.a
	rm -f $@
	$(LDSHARED) -o $@ $(LIBOBJ) $(LIB_DEPS)
	@true

# To make psoriaris make happy we have to mention these files in some
# rule, so we might as well put them here.

mdtest.o: mdtest.c
des_opts.o: des_opts.c
destest.o: destest.c
des.o: des.c
rpw.o: rpw.c
speed.o: speed.c
md5crypt_test.o: md5crypt_test.c

mdtest$(EXECSUFFIX): mdtest.o $(LIB)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ mdtest.o -L. -ldes

md5crypt_test$(EXECSUFFIX): md5crypt_test.o $(LIB)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ md5crypt_test.o -L. -ldes

des_opts$(EXECSUFFIX): des_opts.o set_key.o
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ des_opts.o set_key.o

destest$(EXECSUFFIX): destest.o $(LIB)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ destest.o -L. -ldes

des$(EXECSUFFIX): des.o $(LIB)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ des.o -L. -ldes

rpw$(EXECSUFFIX): rpw.o $(LIB)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ rpw.o -L. -ldes

speed$(EXECSUFFIX): speed.o $(LIB)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ speed.o -L. -ldes

$(LIBOBJ): ../../include/config.h

.PHONY: all Wall install uninstall check clean mostlyclean distclean realclean
