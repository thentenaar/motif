/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <X11/Intrinsic.h>

#include "XmI.h"
#include "XmStringI.h"

static sigjmp_buf segjmp;
static struct sigaction old_seg;
static struct sigaction old_bus;

/**
 * Yes, I'm aware of possible UB around using longjmp() in a signal
 * handler, but it's the only way of avoiding restarted syscalls
 * or loops triggering additional segfaults and getting stuck.
 */
static void seg_handler(int sig)
{
	(void)sig;
	siglongjmp(segjmp, 1);
}

/**
 * Returns True if our handler is currently installed
 */
static Boolean seg_handler_installed(void)
{
	Boolean ret = False;
	struct sigaction old;

	if (!sigaction(SIGSEGV, NULL, &old)) {
		if (old.sa_handler == seg_handler)
		   ret = True;
		sigaction(SIGSEGV, &old, NULL);
	}

	return ret;
}

/**
 * Install our SIGSEGV/SIGBUS handler
 */
static void install_seg_handler(void)
{
	sigset_t mask;
	struct sigaction sa;

	/* Block everything we can while our handler runs */
	sigfillset(&mask);
	memset(&sa, 0, sizeof sa);
	sa.sa_mask    = mask;
	sa.sa_handler = seg_handler;
	sigaction(SIGSEGV, &sa, &old_seg);
	sigaction(SIGBUS,  &sa, &old_bus);
}

/**
 * Remove our handler
 */
static void uninstall_seg_handler(void)
{
	sigaction(SIGSEGV, &old_seg, NULL);
	sigaction(SIGBUS,  &old_bus, NULL);
}

/************************************************************************
 *                                                                      *
 * XmStringIsValid - returns True if the parameter is an XmString.      *
 *                                                                      *
 ************************************************************************/
Boolean XmStringIsValid(const XmString string)
{
	unsigned int len;
	XtPointer val;
	Boolean valid = False;
	volatile XmStringContext ctx;
	XmStringComponentType t;

	if (!string)
		return False;

	_XmProcessLock();
	XmStringInitContext(&ctx, string);

	/**
	 * If this happens, either we've been called recursively from somewhere
	 * below XmeStringGetComponent() which should not happen, or from
	 * another thread and the process lock is non-functional. So, play
	 * it safe and inform the caller that their supposed XmString ain't
	 * valid.
	 */
	if (seg_handler_installed()) {
		XtWarningMsg("recursiveCall", "XmStringIsValid", "XtToolkitError",
		             "Called with signal handler already installed", NULL, NULL);
		goto done;
	}

	/**
	 * Stepping on the signal mask in a toolkit library is less than
	 * desirable, but it's the only way to avoid taking down the entire
	 * process if the caller passed something janky.
	 */
	install_seg_handler();
	if (sigsetjmp(segjmp, True)) {
		uninstall_seg_handler();
		valid = False;
		goto done;
	}

	/* refcount can't be zero */
	if (!_XmStrRefCountGet(string))
		ctx->error = True;

	/* Check components until we get to the end */
	t = XmSTRING_COMPONENT_UNKNOWN;
	while (t != XmSTRING_COMPONENT_END) {
		t = XmeStringGetComponent(ctx, True, False, &len, &val);
		if (t == XmSTRING_COMPONENT_UNKNOWN) {
			ctx->error = True;
			break;
		}
	}
	uninstall_seg_handler();

	/* If we didn't find an error, it's valid enough */
	valid = !ctx->error;

done:
	XmStringFreeContext(ctx);
	_XmProcessUnlock();
	return valid;
}

/**
 * Return True if we can grok each component in the stream without
 * incurring any memory access errors
 */
Boolean XmStringSerializedIsValid(const unsigned char *stream)
{
	size_t i, len, bytes, total, remaining;
	Boolean valid = False;

	if (!stream || *stream != 0xdf)
		return False;

	_XmProcessLock();
	if (seg_handler_installed()) {
		XtWarningMsg("recursiveCall", "XmStringByteStreamIsValid", "XtToolkitError",
		             "Called with signal handler already installed", NULL, NULL);
		goto done;
	}

	install_seg_handler();
	if (sigsetjmp(segjmp, True)) {
		valid = False;
		goto done;
	}

	if (stream[1] != 0x80)
		goto done;

	/* Read the overall length, and advance */
	total   = 0;
	bytes   = stream[3];
	stream += 4;

	if (bytes & 0x80) {
		bytes &= 0x7f;
		do {
			total = (total << 8) | *stream++;
		} while (--bytes);
	} else total = bytes & 0x7f;

	remaining = total;
	while (remaining > 0 && remaining <= total) {
		/* Figure component length and advance */
		len = *++stream & 0x7f;
		if (len & 0x80) {
			bytes = len & 0x7f;
			len   = 0;
			do {
				len = (len << 8) | *stream++;
				remaining--;
			} while (--bytes);
		}

		stream    += len + 1;
		remaining -= len + 2;
	}

	/* Absent any mishaps... */
	valid = !remaining;

done:
	uninstall_seg_handler();
	_XmProcessUnlock();
	return valid;
}

