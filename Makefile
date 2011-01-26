prefix=/usr
bindir = ${prefix}/bin
mandir = ${prefix}/share/man
INSTALL = /bin/install
INSTALLPROG = ${INSTALL} -m 0755
INSTALLMAN = ${INSTALL} -m 0644

all:
	make -C xembed xembed.o
	make -C trayion

clean:
	make -C xembed clean
	make -C trayion clean

install: trayion/trayion
	mkdir -p ${bindir}
	mkdir -p ${mandir}/man1
	${INSTALLPROG} trayion/trayion ${bindir}/trayion
	${INSTALLMAN} doc/trayion.1 ${mandir}/man1/trayion.1
