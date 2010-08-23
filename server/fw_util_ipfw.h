/* $Id$
 *****************************************************************************
 *
 * File:    fw_util_ipfw.h
 *
 * Author:  Damien Stuart (dstuart@dstuart.org)
 *
 * Purpose: Header file for fw_util_ipfw.c.
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
#ifndef FW_UTIL_IPFW_H
#define FW_UTIL_IPFW_H

/* ipfw command args
*/
#define IPFW_ADD_RULE_ARGS        "add %u set %u pass %u from %s to me dst-port %u setup keep-state // _exp_%u"
#define IPFW_ADD_CHECK_STATE_ARGS "add %u set %u check-state"
#define IPFW_MOVE_RULE_ARGS       "set move rule %u to %u"
#define IPFW_MOVE_SET_ARGS        "set move %u to %u"
#define IPFW_DEL_RULE_ARGS        "set %u delete %u"
#define IPFW_DEL_RULE_SET_ARGS    "delete set %u"
#define IPFW_LIST_RULES_ARGS      "-d -S -T set %u list"

#endif /* FW_UTIL_IPFW_H */

/***EOF***/
