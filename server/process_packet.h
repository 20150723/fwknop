/* $Id$
 *****************************************************************************
 *
 * File:    process_packet.h
 *
 * Author:  Damien Stuart (dstuart@dstuart.org)
 *
 * Purpose: Header file for process_packet and other fwknopd code.
 *
 * Copyright (C) 2009 Damien Stuart (dstuart@dstuart.org)
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
#ifndef PROCESS_PACKET_H
#define PROCESS_PACKET_H

#define IPV4_VER_MASK   0x15
#define MIN_IPV4_WORDS  0x05

/* For items not defined by this system
*/
#ifndef ETHER_CRC_LEN
  #define ETHER_CRC_LEN 4
#endif
#ifndef ETHER_HDR_LEN
  #define ETHER_HDR_LEN 14
#endif

/* Prototypes
*/
void process_packet(unsigned char *args, const struct pcap_pkthdr *packet_header, const unsigned char *packet);

#endif  /* PROCESS_PACKET_H */
