#
# Debugging output level. 0 suppresses output, 1 gives some diagnostics,
# 2 is more verbose.
TRACE_LEVEL = 0

#
# X11 install location.
X11_PREFIX = /usr/X11R6

#
# X compiler and linker flags.
X11CFLAGS = -I${X11_PREFIX}/include
X11LDFLAGS = -L${X11_PREFIX}/lib -lX11
