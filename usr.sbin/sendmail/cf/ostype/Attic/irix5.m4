divert(-1)
#
# Copyright (c) 1998 Sendmail, Inc.  All rights reserved.
# Copyright (c) 1995 Eric P. Allman.  All rights reserved.
# Copyright (c) 1988, 1993
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#
#  Contributed by Kari E. Hurtta <Kari.Hurtta@dionysos.fmi.fi>
#

#
# Notes:
# - SGI's /etc/sendmail.cf defines also 'u' for local mailer flags -- you
#   perhaps don't want it.
# - Perhaps is should also add define(`LOCAL_MAILER_CHARSET', iso-8859-1)
#   put some Asian sites may prefer otherwise -- or perhaps not.
# - SGI's /etc/sendmail.cf seems use: A=mail -s -d $u
#   It seems work without that -s however.
# - SGI's /etc/sendmail.cf set's default uid and gid to 998 (guest)
# - In SGI seems that TZ variable is needed that correct time is marked to
#   syslog
# - helpfile is in /etc/sendmail.hf in SGI's /etc/sendmail.cf
#

divert(0)
VERSIONID(`@(#)irix5.m4	8.11 (Berkeley) 10/6/1998')
ifdef(`LOCAL_MAILER_FLAGS',, `define(`LOCAL_MAILER_FLAGS', Ehmu9)')dnl
ifdef(`LOCAL_MAILER_ARGS',, `define(`LOCAL_MAILER_ARGS', `mail -s -d $u')')dnl
ifdef(`QUEUE_DIR',, `define(`QUEUE_DIR', /var/spool/mqueue)')dnl
define(`ALIAS_FILE', ifdef(`_USE_ETC_MAIL_', `/etc/mail/aliases', `/etc/aliases'))dnl
ifdef(`STATUS_FILE',, `define(`STATUS_FILE', ifdef(`_USE_ETC_MAIL_', `/etc/mail/statistics', `/var/sendmail.st'))')dnl
ifdef(`HELP_FILE',, `define(`HELP_FILE', ifdef(`_USE_ETC_MAIL_', `/etc/mail/helpfile', `/etc/sendmail.hf'))')dnl
define(`confDEF_USER_ID', `998:998')dnl
define(`confTIME_ZONE', USE_TZ)dnl
define(`confEBINDIR', `/usr/lib')dnl
