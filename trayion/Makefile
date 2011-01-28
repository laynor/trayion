include ../Rules.make

CC = gcc
CFLAGS = ${X11CFLAGS} -DTRACE_LEVEL=${TRACE_LEVEL} -I../xembed -Wall -g
LDFLAGS = ${X11LDFLAGS}

OBJS = main.o ui.o systray.o list_sort.o sorted_classes.o hidden_list.o string_list.o configuration_files.o
XEMBED_OBJS = ../xembed/xembed.o

trayion: ${OBJS} ${XEMBED_OBJS}
	gcc ${OBJS} ${XEMBED_OBJS} -o trayion ${LDFLAGS}

clean:
	rm -f trayion ${OBJS}
