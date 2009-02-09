/*
 ******************************************************************************
 *
 * File:    fwknop_common.h
 *
 * Author:  Damien Stuart
 *
 * Purpose: Header file for fwknop config_init.
 *
 * Copyright (C) 2008 Damien Stuart (dstuart@dstuart.org)
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
 ******************************************************************************
*/
#ifndef FWKNOP_COMMON_H
#define FWKNOP_COMMON_H

/* Common includes for our other fwknop client source files.
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "fko.h"

/* My Name and Version
*/
#define MY_NAME     "fwknop"
#define MY_DESC     "Single Packet Authorization client"

#define MAJOR_VER   "1"
#define MINOR_VER   "10"
#define MICRO_VER   "0"
#define MY_VERSION  MAJOR_VER"."MINOR_VER"."MICRO_VER

/* Default config path, can override with -c
*/
#define DEF_CONFIG_FILE MY_NAME".conf"

/* Other common defines
*/
#define FKO_PW "BubbaWasHere" //--DSS Temp

#define FKO_DEFAULT_PROTO IPPROTO_UDP
#define FKO_DEFAULT_PORT 62201
#define MAX_IP_STR_LEN 16

#define MAX_PATH_LEN    1024
#define MAX_GPG_KEY_ID  128

/* fwkop client configuration parameters and values
*/
typedef struct fko_cli_options
{
    char config_file[MAX_PATH_LEN];
    char spa_server_ip_str[MAX_IP_STR_LEN];
    char spoof_ip_src_str[MAX_IP_STR_LEN];
    char gpg_recipient_key[MAX_GPG_KEY_ID];
    char gpg_signer_key[MAX_GPG_KEY_ID];

    int proto;
    unsigned int port;
    unsigned int src_port;  /* only used with --source-port */

    unsigned int digest_type;

    /* Various command-line flags */
    unsigned char   debug;
    unsigned char   quiet;   /* --quiet mode */
    unsigned char   verbose; /* --verbose mode */
    unsigned char   version; /* --version */
    unsigned char   test;
    unsigned char   use_gpg;
    unsigned char   use_gpg_agent;

    //char            config_file[MAX_PATH_LEN];

} fko_cli_options_t;

extern fko_cli_options_t options;

#endif /* FWKNOP_COMMON_H */

/***EOF***/
