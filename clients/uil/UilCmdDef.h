/**
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
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This include file defines the interface to the UIL command line
**	processing.
**
**--
**/

#ifndef UilCmdDef_h
#define UilCmdDef_h

typedef unsigned int (*uil_message_cb_t)(const char *message_data,
                                         int message_number,
                                         int severity,
                                         const char *msg_buffer,
                                         const char *src_buffer,
                                         const char *ptr_buffer,
                                         const char *loc_buffer,
                                         int message_cnt[]);

typedef unsigned int (*uil_status_cb_t)(const char *status_data,
                                        int pct_complete,
                                        int lines_processed,
                                        const char *current_file,
                                        int message_cnt[]);

typedef struct
{
    char	    *ac_source_file;
    char	    *ac_resource_file;
    char	    *ac_listing_file;
    unsigned int    include_dir_count;	    /* number of directories in	    */
					    /* ac_include_dir array	    */
    char	    **ac_include_dir;    /* pointer to array of	    */
					    /* directories to search for    */
					    /* includes in		    */
    unsigned	    v_listing_file: 1;
    unsigned	    v_resource_file: 1;
    unsigned	    v_show_machine_code: 1;
    unsigned	    v_report_info_msg: 1;
    unsigned	    v_report_warn_msg: 1;
    unsigned	    v_parse_tree: 1;	    /* generate parse tree	    */
    unsigned        v_issue_summary: 1;

    unsigned int    status_update_delay;    /* Number of times a status	    */
					    /* point is passed before	    */
					    /* calling statusCB routine	    */
					    /* 0 means called every time    */
    uil_message_cb_t message_cb;
    char		*message_data;
    uil_status_cb_t status_cb;
    char		*status_data;
    unsigned	    	v_use_setlocale: 1;    /* Allow localized strings */
    char		*ac_database;
    unsigned 		v_database: 1;
} cmd_command_line_type;

#endif /* UilCmdDef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
