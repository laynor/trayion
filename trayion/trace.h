/*******************************************************************************
 trace.h

 Non-obtrusive, switchable debugging output.


 This source code is copyright (C) 2002, 2003 Matthew Reppert.
 Use of this code is permitted under the terms of the GNU General Public
 License v2.
 ******************************************************************************/

#ifndef ICE_TRACE_H
#define ICE_TRACE_H

#if TRACE_LEVEL == 3
 #include <stdio.h>
 #define TRACE3(args)	fprintf args
 #define TRACE2(args)	fprintf args
 #define TRACE1(args)	fprintf args
 #define TRACE(args)	fprintf args
#elif TRACE_LEVEL == 2
 #include <stdio.h>
 #define TRACE3(args)
 #define TRACE2(args)	fprintf args
 #define TRACE1(args)	fprintf args
 #define TRACE(args)	fprintf args
#elif TRACE_LEVEL == 1
 #include <stdio.h>
 #define TRACE3(args)
 #define TRACE2(args)
 #define TRACE1(args)	fprintf args
 #define TRACE(args)	fprintf args
#else
 #define TRACE3(args)
 #define TRACE2(args)
 #define TRACE1(args)
 #define TRACE(args)
#endif

#endif
