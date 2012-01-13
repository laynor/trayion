include ../Rules.make

CC = gcc
CFLAGS = ${X11CFLAGS} -DTRACE_LEVEL=${TRACE_LEVEL} -I../xembed -Wall -g
LDFLAGS = ${X11LDFLAGS}

OBJS = main.o ui.o systray.o list_sort.o sorted_classes.o hidden_list.o string_list.o configuration_files.o xutils.o
XEMBED_OBJS = ../xembed/xembed.o

all: trayion #trayion-client

trayion: ${OBJS} ${XEMBED_OBJS}
	gcc ${OBJS} ${XEMBED_OBJS} -o trayion ${LDFLAGS}

trayion-client: trayion-client.o
	gcc trayion-client.o -o trayion-client ${LDFLAGS}

trayion-client.o: trayion-client.c
	gcc ${CFLAGS} -c trayion-client.c

clean:
	rm -f trayion ${OBJS}
