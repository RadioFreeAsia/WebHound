## automake.am
##
## Makefile for the whprocess(1) log processor script
##
## (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4CLI_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@


bin_PROGRAMS = whprocess

dist_whprocess_SOURCES = clfentry.cpp clfentry.h\
                         whprocess.cpp whprocess.h

nodist_whprocess_SOURCES = cmdswitch.cpp cmdswitch.h\
                           config.cpp config.h\
                           db.cpp db.h\
                           moc_db.cpp\
                           moc_whprocess.cpp\
                           profile.cpp profile.h

whprocess_LDADD = @QT4CLI_LIBS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk
DISTCLEANFILES = cmdswitch.cpp cmdswitch.h\
                 config.cpp config.h\
                 db.cpp db.h\
                 profile.cpp profile.h

MAINTAINERCLEANFILES = *~\
                       Makefile.in
