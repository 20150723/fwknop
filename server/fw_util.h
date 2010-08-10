/* $Id$
 *****************************************************************************
 *
 * File:    fw_util.h
 *
 * Author:  Damien Stuart (dstuart@dstuart.org)
 *
 * Purpose: Header file for fw_util.c.
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
#ifndef FW_UTIL_H
#define FW_UTIL_H

#define CMD_BUFSIZE                 256
#define MAX_FW_COMMAND_ARGS_LEN     256

#define STANDARD_CMD_OUT_BUFSIZE    4096

#if FIREWALL_IPTABLES
  #include "fw_util_iptables.h"
#elif FIREWALL_IPFW
  #include "fw_util_ipfw.h"
#elif FIREWALL_IPF
  #include "fw_util_ipf.h"
#endif

#if HAVE_TIME_H
  #include <time.h>
#endif

/* Function prototypes
*/
void fw_config_init(fko_srv_options_t *opts);
void fw_initialize(void);
void fw_cleanup(void);
void check_firewall_rules(fko_srv_options_t *opts);
int fw_dump_rules(fko_srv_options_t *opts);
int process_spa_request(fko_srv_options_t *opts, spa_data_t *spdat);

#endif /* FW_UTIL_H */

/***EOF***/
