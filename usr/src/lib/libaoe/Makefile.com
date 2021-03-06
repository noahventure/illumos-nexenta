#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet at
# http://www.illumos.org/license/CDDL.
#

#
# Copyright 2012 Nexenta Systems, Inc.  All rights reserved.
#

LIBRARY=	libaoe.a
VERS=		.1

OBJECTS=	libaoe.o

include		../../Makefile.lib

CPPFLAGS +=	-I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include

LIBS=		$(DYNLIB) $(LINTLIB)

SRCDIR=		../common

LDLIBS +=	-lc -ldladm -lscf

$(DYNLIB) :=	LDLIBS += -ldbus-1

$(LINTLIB) :=	SRCS=	$(SRCDIR)/$(LINTSRC)

.KEEP_STATE:

all:		$(LIBS)

lint:		lintcheck

include		../../Makefile.targ
