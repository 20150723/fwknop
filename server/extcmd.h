/* $Id$
 *****************************************************************************
 *
 * File:    extcmd.h
 *
 * Author:  Damien Stuart (dstuart@dstuart.org)
 *
 * Purpose: Header file for extcmd.c.
 *
 * Copyright (C) 2010 Damien Stuart (dstuart@dstuart.org)
 *
 *  License (GNU Public License):
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *     USA
 *
 *****************************************************************************
*/
#ifndef EXTCMD_H
#define EXTCMD_H

#define IO_READ_BUF_LEN     256
#define EXTCMD_DEF_TIMEOUT  15

/* The various return status states in which an external command result
 * may end up in.
*/
enum {
    EXTCMD_SETUID_ERROR             =   -4,
    EXTCMD_SELECT_ERROR             =   -3,
    EXTCMD_PIPE_ERROR               =   -2,
    EXTCMD_FORK_ERROR               =   -1,
    EXTCMD_SUCCESS_ALL_OUTPUT       = 0x00,
    EXTCMD_SUCCESS_PARTIAL_STDOUT   = 0x01,
    EXTCMD_SUCCESS_PARTIAL_STDERR   = 0x02,
    EXTCMD_STDOUT_READ_ERROR        = 0x04,
    EXTCMD_STDERR_READ_ERROR        = 0x08,
    EXTCMD_EXECUTION_ERROR          = 0x10,
    EXTCMD_EXECUTION_TIMEOUT        = 0x20
};

/* Some convenience macros for testing the extcmd return status.
*/
#define EXTCMD_IS_SUCCESS(x) (x == EXTCMD_SUCCESS_ALL_OUTPUT)
#define EXTCMD_IS_SUCCESS_PARTIAL_STDOUT(x) (x && EXTCMD_SUCCESS_PARTIAL_STDOUT)
#define EXTCMD_IS_SUCCESS_PARTIAL_STDERR(x) (x && EXTCMD_SUCCESS_PARTIAL_STDERR)
#define EXTCMD_IS_SUCCESS_PARTIAL_OUTPUT(x) \
    ((x && EXTCMD_SUCCESS_PARTIAL_STDOUT) \
        || x && EXTCMD_SUCCESS_PARTIAL_STDERR)
#define EXTCMD_STDOUT_READ_ERROR(x) (x && EXTCMD_STDOUT_READ_ERROR)
#define EXTCMD_STDERR_READ_ERROR(x) (x && EXTCMD_STDERR_READ_ERROR)
#define EXTCMD_READ_ERROR(x) \
    ((x && EXTCMD_STDOUT_READ_ERROR) \
        || x && EXTCMD_STDERR_READ_ERROR)
#define EXTCMD_EXECUTION_ERROR(x) (x && EXTCMD_EXECUTION_ERROR)

#define EXTCMD_NOERROR(x,y) ((y == 0) \
    && (EXTCMD_IS_SUCCESS(x) || EXTCMD_IS_SUCCESS_PARTIAL_OUTPUT(x))

/* Function prototypes
*/
int run_extcmd(char *cmd, char *so_buf, size_t so_buf_sz, int timeout);
int run_extcmd_as(uid_t uid, char *cmd, char *so_buf, size_t so_buf_sz, int timeout);

#endif /* EXTCMD_H */

/***EOF***/
