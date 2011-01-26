include ../Rules.make

CFLAGS = -DTRACE_LEVEL=${TRACE_LEVEL} ${X11CFLAGS} -Wall -g
LDFLAGS = ${X11LDFLAGS}

all: xembed.o

clean:
	rm -f xembed.o
