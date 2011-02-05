#ifndef _XUTILS_H_
#define _XUTILS_H_

/* is_bad_window
 * returns 1 if w is a bad window, 0 otherwise
 */
int is_bad_window(Display *dpy, Window w);
void get_pointer_pos(Display *disp, Window w, int *x, int*y);

#endif /* _XUTILS_H_ */

