/*
 ******************************************************************************
 *
 * File:    config_init.h
 *
 * Author:  Damien Stuart
 *
 * Purpose: Header file for fwknopd config_init.
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
 ******************************************************************************
*/
#ifndef CONFIG_INIT_H
#define CONFIG_INIT_H

#include <getopt.h>
#include <sys/stat.h>

/* Some convenience macros */

/* Characters allowed between a config parameter and its value.
*/
#define IS_CONFIG_PARAM_DELIMITER(x) (x == ' ' || x == '\t' || x == '=');

/* End of line characters.
*/
#define IS_LINE_END(x) (x == '\n' || x == '\r' || x == ';');

/* Characters in the first position of a line that make it considered
 * empty or otherwise non-interesting (like a comment).
*/
#define IS_EMPTY_LINE(x) ( \
    x == '#' || x == '\n' || x == '\r' || x == ';' || x == '\0' \
)

/* String compare macro.
*/
#define CONF_VAR_IS(n, v) (strcmp(n, v) == 0)

/* Long options values (for those that may not have a short option).
*/
enum {
    FW_LIST         = 0x200,
    GPG_HOME_DIR,
    ROTATE_DIGEST_CACHE,
    NOOP /* Just to be a marker for the end */
};

/* Our getopt_long options string.
*/
#define GETOPTS_OPTION_STRING "a:c:C:Dfhi:Kl:O:RSvV"

/* Our program command-line options...
*/
static struct option cmd_opts[] =
{
    {"access-file",         1, NULL, 'a'},
    {"config-file",         1, NULL, 'c'},
    {"packet-limit",        1, NULL, 'C'},
    {"dump-config",         0, NULL, 'D'},
    {"foreground",          0, NULL, 'f'},
    {"help",                0, NULL, 'h'},
    {"interface",           1, NULL, 'i'},
    {"kill",                0, NULL, 'K'},
    {"fw-list",             0, NULL, FW_LIST },
    {"gpg-home-dir",        1, NULL, GPG_HOME_DIR },
    {"locale",              1, NULL, 'l' },
    {"rotate-digest-cache", 0, NULL, ROTATE_DIGEST_CACHE },
    {"override-config",     1, NULL, 'O' },
    {"restart",             0, NULL, 'R'},
    {"status",              0, NULL, 'S'},
    {"verbose",             0, NULL, 'v'},
    {"version",             0, NULL, 'V'},
    {0, 0, 0, 0}
};

/* Function Prototypes
*/
void config_init(fko_srv_options_t *opts, int argc, char **argv);
void dump_config(fko_srv_options_t *opts);
void clear_configs(fko_srv_options_t *opts);
void free_configs(fko_srv_options_t *opts);
void usage(void);

#endif /* CONFIG_INIT_H */

/***EOF***/
