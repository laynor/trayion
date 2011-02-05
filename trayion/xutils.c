#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>

#include "xutils.h"

static int (*old_error_handler)(Display *, XErrorEvent *) = NULL;
static int bad_window=0;

int is_bad_window_handler(Display *dpy, XErrorEvent *e)
{
	if(e->error_code == BadWindow){
		bad_window = 1;
	}else{
		bad_window = 0;
		assert(old_error_handler);
		(*old_error_handler)(dpy, e);
	}
	return 0;
}

int is_bad_window(Display *dpy, Window w)
{
	XWindowAttributes attr;
	bad_window = 0;
	XSync(dpy, False);
	old_error_handler = XSetErrorHandler(is_bad_window_handler);
	XGetWindowAttributes(dpy, w, &attr);
	XSync(dpy, False);
	XSetErrorHandler(old_error_handler);
	return bad_window;
}

void get_pointer_pos(Display *disp, Window w, int *x, int*y)
{
	int root_x, root_y;
	unsigned int pointer_mask;
	Window child_return, root_return;
	XQueryPointer(disp, w, &root_return, &child_return, 
		      &root_x, &root_y,
		      x, y, &pointer_mask);
}
