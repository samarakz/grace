#####################################################
# Makefile for Qt Grace                             #
#####################################################
# You should not change anything here.              #
#####################################################

all : lib

install : lib

tests : dummy

check : dummy

Makefile.qt : qtgrace.pro
	qmake -o Makefile.qt qtgrace.pro

lib : Makefile.qt
	$(MAKE) -f Makefile.qt

clean: Makefile.qt
	$(MAKE) -f Makefile.qt clean

distclean: Makefile.qt
	$(MAKE) -f Makefile.qt distclean
	$(RM) Makefile.qt
	$(RM) object_script.libqtgrace

dummy :
