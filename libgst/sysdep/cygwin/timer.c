/******************************** -*- C -*- ****************************
 *
 * System specific implementation module.
 *
 * This module contains implementations of various operating system
 * specific routines.  This module should encapsulate most (or all)
 * of these calls so that the rest of the code is portable.
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 1988,89,90,91,92,94,95,99,2000,2001,2002,2003,2006,2007,2008,2009
 * Free Software Foundation, Inc.
 * Written by Steve Byrne.
 *
 * This file is part of GNU Smalltalk.
 *
 * GNU Smalltalk is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * Linking GNU Smalltalk statically or dynamically with other modules is
 * making a combined work based on GNU Smalltalk.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the Free Software Foundation
 * give you permission to combine GNU Smalltalk with free software
 * programs or libraries that are released under the GNU LGPL and with
 * independent programs running under the GNU Smalltalk virtual machine.
 *
 * You may copy and distribute such a system following the terms of the
 * GNU GPL for GNU Smalltalk and the licenses of the other code
 * concerned, provided that you include the source code of that other
 * code when and as the GNU GPL requires distribution of source code.
 *
 * Note that people who make modified versions of GNU Smalltalk are not
 * obligated to grant this special exception for their modified
 * versions; it is their choice whether to do so.  The GNU General
 * Public License gives permission to release a modified version without
 * this exception; this exception also makes it possible to release a
 * modified version which carries forward this exception.
 *
 * GNU Smalltalk is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GNU Smalltalk; see the file COPYING.	 If not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ***********************************************************************/


#include "gstpriv.h"

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN /* avoid including junk */
# include <windows.h>
#endif

struct
{
  HANDLE hNewWaitEvent;
  long sleepTime;
}
alarms;

/* thread for precise alarm callbacks */
void CALLBACK
alarm_thread (unused)
     LPVOID unused;
{
  WaitForSingleObject (alarms.hNewWaitEvent, INFINITE);
  for (;;)
    {
      int sleepTime;

      sleepTime = alarms.sleepTime;
      if (sleepTime > 0)
	{
	  if (WaitForSingleObject (alarms.hNewWaitEvent, sleepTime) !=
	      WAIT_TIMEOUT)
	    {
	      /* The old wait was canceled by a new one */
	      continue;
	    }
	}
      raise (SIGALRM);
      WaitForSingleObject (alarms.hNewWaitEvent, INFINITE);
    }
}

void
_gst_init_sysdep_win32 ()
{
  HANDLE hthread;
  DWORD tid;

  /* Starts as non-signaled, so alarm_thread will wait */
  alarms.hNewWaitEvent = CreateEvent (NULL, FALSE, FALSE, NULL);

  /* Start alarm_thread with a 1024 bytes stack */
  hthread = CreateThread (NULL,
			  1024,
			  (LPTHREAD_START_ROUTINE) alarm_thread,
			  NULL, 0, &tid);
  
  /* This does not terminate the thread - it only releases our handle */
  CloseHandle (hthread);
}

/* Please feel free to make this more accurate for your operating system
 * and send me the changes.
 */
void
_gst_signal_after (int deltaMilli,
		   SigHandler func,
		   int kind)
{
  if (func)
    _gst_set_signal_handler (kind, func);

  if (deltaMilli <= 0)
    {
      raise (kind);
      return;
    }

#ifdef SIGVTALRM
  if (kind == SIGVTALRM)
    {
#if defined ITIMER_VIRTUAL
      struct itimerval value;
      value.it_interval.tv_sec = value.it_interval.tv_usec = 0;
      value.it_value.tv_sec = deltaMilli / 1000;
      value.it_value.tv_usec = (deltaMilli % 1000) * 1000;
      setitimer (ITIMER_VIRTUAL, &value, (struct itimerval *) 0);
#endif
    }
#endif

  if (kind == SIGALRM)
    {
      alarms.sleepTime = deltaMilli;
      SetEvent (alarms.hNewWaitEvent);
    }
}
