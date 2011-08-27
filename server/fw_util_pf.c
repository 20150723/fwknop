/*
 *****************************************************************************
 *
 * File:    fw_util_pf.c
 *
 * Author:  Damien S. Stuart, Michael Rash
 *
 * Purpose: Fwknop routines for managing pf firewall rules.
 *
 * Copyright 2010 Damien Stuart (dstuart@dstuart.org)
 *
 *  License (GNU Public License):
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *****************************************************************************
*/
#include "fwknopd_common.h"

#if FIREWALL_PF

#include "fw_util.h"
#include "utils.h"
#include "log_msg.h"
#include "extcmd.h"
#include "access.h"

static struct fw_config fwc;
static char   cmd_buf[CMD_BUFSIZE];
static char   err_buf[CMD_BUFSIZE];
static char   cmd_out[STANDARD_CMD_OUT_BUFSIZE];

static void
zero_cmd_buffers(void)
{
    memset(cmd_buf, 0x0, CMD_BUFSIZE);
    memset(err_buf, 0x0, CMD_BUFSIZE);
    memset(cmd_out, 0x0, STANDARD_CMD_OUT_BUFSIZE);
}

/* Print all firewall rules currently instantiated by the running fwknopd
 * daemon to stdout.
*/
int
fw_dump_rules(fko_srv_options_t *opts)
{
    int     i = 0;
    int     res, got_err = 0;

    zero_cmd_buffers();

    /* Create the list command for active rules
    */
    snprintf(cmd_buf, CMD_BUFSIZE-1, "%s " PF_LIST_RULES_ARGS,
        opts->fw_config->fw_command,
        opts->fw_config->anchor
    );

    printf("\nActive Rules in PF anchor: %s:\n", opts->fw_config->anchor);
    res = system(cmd_buf);

    /* Expect full success on this */
    if(! EXTCMD_IS_SUCCESS(res))
    {
        log_msg(LOG_ERR, "Error %i from cmd:'%s': %s", res, cmd_buf, err_buf); 
        got_err++;
    }

    return(got_err);
}

void
fw_config_init(fko_srv_options_t *opts)
{
    memset(&fwc, 0x0, sizeof(struct fw_config));

    /* Set our firewall exe command path (iptables in most cases).
    */
    strlcpy(fwc.fw_command, opts->config[CONF_FIREWALL_EXE], MAX_PATH_LEN);

    /* Set the PF anchor name
    */
    strlcpy(fwc.anchor, opts->config[CONF_PF_ANCHOR_NAME], MAX_PF_ANCHOR_LEN);

    /* Let us find it via our opts struct as well.
    */
    opts->fw_config = &fwc;

    return;
}

void
fw_initialize(fko_srv_options_t *opts)
{
    int res = 0;

    if(res != 0)
    {
        fprintf(stderr, "Warning: Errors detected during fwknop custom chain creation.\n");
        exit(EXIT_FAILURE);
    }
}

int
fw_cleanup(void)
{
    return(0);
}

/****************************************************************************/

/* Rule Processing - Create an access request...
*/
int
process_spa_request(fko_srv_options_t *opts, spa_data_t *spadat)
{
    char             nat_ip[16] = {0};
    char            *ndx;

    unsigned int     nat_port = 0;;

    acc_port_list_t *port_list = NULL;
    acc_port_list_t *ple;

    unsigned int    fst_proto;
    unsigned int    fst_port;

    int             res = 0;
    time_t          now;
    unsigned int    exp_ts;

    /* Parse and expand our access message.
    */
    expand_acc_port_list(&port_list, spadat->spa_message_remain);

    /* Start at the top of the proto-port list...
    */
    ple = port_list;

    /* Remember the first proto/port combo in case we need them
     * for NAT access requests.
    */
    fst_proto = ple->proto;
    fst_port  = ple->port;

    /* Set our expire time value.
    */
    time(&now);
    exp_ts = now + spadat->fw_access_timeout;

    return(res);
}

/* Iterate over the configure firewall access chains and purge expired
 * firewall rules.
*/
void
check_firewall_rules(fko_srv_options_t *opts)
{
    char             exp_str[12];
    char             rule_num_str[6];
    char            *ndx, *rn_start, *rn_end, *tmp_mark;

    int             i, res, rn_offset;
    time_t          now, rule_exp, min_exp = 0;

    time(&now);

    zero_cmd_buffers();
}

#endif /* FIREWALL_PF */

/***EOF***/
