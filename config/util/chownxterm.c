/* $TOG: chownxterm.c /main/4 1998/02/06 11:23:10 kaleb $ */
/*

 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
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

/*
 * chownxterm --- make xterm suid root
 *
 * By Stephen Gildea, December 1993
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define XTERM_PATH "/x11/programs/xterm/xterm"

#include <stdio.h>
#include <errno.h>

char *prog_name;

void help()
{
    setgid(getgid());
    setuid(getuid());
    printf("chown-xterm makes %s suid root\n", XTERM_PATH);
    printf("This is necessary on Ultrix for /dev/tty operation.\n");
    exit(0);
}

void print_error(err_string)
    char *err_string;
{
    setgid(getgid());
    setuid(getuid());
    fprintf(stderr, "%s: \"%s\"", prog_name, err_string);
    perror(" failed");
    exit(1);
}

main(argc, argv)
    int argc;
    char **argv;
{
    prog_name = argv[0];
    if (argc >= 2 && strcmp(argv[1], "-help") == 0) {
	help();
    } else {
	if (chown(XTERM_PATH, 0, -1) != 0)
	    print_error("chown root " XTERM_PATH);
	if (chmod(XTERM_PATH, 04555) != 0)
	    print_error("chmod 4555 " XTERM_PATH);
    }
    exit(0);
}
