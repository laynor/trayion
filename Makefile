prefix=/usr
bindir = ${prefix}/bin
mandir = ${prefix}/share/man
INSTALL = /usr/bin/install
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
	${INSTALLPROG} trayion-dump.sh ${bindir}/trayion-dump
	${INSTALLPROG} trayion-reload.sh ${bindir}/trayion-reload
	${INSTALLPROG} trayion-crash-kludge.sh ${bindir}/trayion-crash-kludge.sh
	${INSTALLMAN} doc/trayion.1 ${mandir}/man1/trayion.1
