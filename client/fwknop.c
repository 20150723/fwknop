/* $Id$
 *****************************************************************************
 *
 * File:    fwknop.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: fwknop client program (or will be evenually :).
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
 *****************************************************************************
*/
#include "fwknop.h"

/* Local prototypes
*/
void init_spa_message(spa_message_t *sm);
void dump_spa_message_data(spa_message_t *sm);

int main(int argc, char **argv)
{
    spa_message_t   sm;
    int             enc_size;
    char            spa_pkt_raw[1024] = {0}; // -DSS what is max size?
    uchar           spa_pkt_enc[1500] = {0}; // -DSS what is max size?
    char            spa_pkt_b64[1500] = {0}; // -DSS what is max size?

    char           *spb64_p = spa_pkt_b64;

    char           *test_pw = "BubbaWasHere";

    /* Initialize - this sets random, user, and other defaults.
    */
    init_spa_message(&sm);

    /* Timestamp
    */
    spa_timestamp(&sm, 0);
    
    /* Construct the spa message based on what we have so far.
    */
    spa_message(&sm);

    /* Create the digest for this message.
    */
    spa_digest(&sm);

    /* Dump the spa_message struct - temp for testing.
    */
    dump_spa_message_data(&sm);

    /* Now take a shot at constructing the packet data.
     * --DSS this is just a slap-together for testing at this point.
     *       I do not think we will do it this way in the end.  :)
    */
    sprintf(spa_pkt_raw, "%s:%s", sm.message, sm.digest);

    /* Encrypt it
    */
    enc_size = fko_encrypt((uchar*)spa_pkt_raw, strlen(spa_pkt_raw), test_pw, spa_pkt_enc);

    /* Base64 encode it and strip off trailing '='s
    */
    b64_encode(spa_pkt_enc, spb64_p, enc_size);
    strip_b64_eq(spb64_p);

    /* Remove the preceeding encoded "Salted__" string (if it is there).
    */
    if(strncmp(spb64_p, "U2FsdGVkX1", 10) == 0)
        spb64_p += 10;

    printf("Hexdump of encrypted data: (%i bytes)\n", enc_size);
    hex_dump(spa_pkt_enc, enc_size);

    printf("Base64 version:\n\n%s\n\n", spb64_p);

    return(0);
} 

/* Initialize the spa_message data struct, and set some default/preliminary
 * values.
*/
void init_spa_message(spa_message_t *sm)
{
    /* Zero our SPA message struct.
    */
    memset(sm, 0x0, sizeof(spa_message_t));

    /* Initialize default values.
    */
    sm->digest_type     = DEFAULT_DIGEST;
    sm->enc_pcap_port   = DEFAULT_PORT;
    sm->message_type    = DEFAULT_MSG_TYPE;
    sm->client_timeout  = DEFAULT_CLIENT_TIMEOUT;

    strlcpy(sm->access_str, DEFAULT_ACCESS_STR, MAX_ACCESS_STR_SIZE);
    strlcpy(sm->allow_ip, DEFAULT_ALLOW_IP, MAX_IP_ADDR_SIZE);

    /* Go ahead and and setup the random and user fields.
    */
    spa_random_number(sm);
    spa_user(sm, NULL);

    /* Version is static, so we add it here as well.
    */
    spa_version(sm);
}

/* Pretty print the data in the spa_message data struct.
*/
void dump_spa_message_data(spa_message_t *sm)
{
    printf(
        "\nCurrent SPA Message Data:\n\n"
        "      Random Val: '%s'\n"
        "            User: '%s'\n"
        "       Timestamp: '%u'\n"
        "         Version: '%s'\n"
        "    Message Type: '%u'\n"
        "          Access: '%s'\n"
        "        Allow IP: '%s'\n"
        "      Nat Access: '%s'\n"
        "     Server Auth: '%s'\n"
        "  Client Timeout: '%u'\n"
        "         Message: '%s'\n"
        "          Digest: '%s'\n"
        "\n"
        "     Digest Type: '%u'\n"
        "            Port: '%u'\n"
        "\n",
            sm->rand_val,
            sm->user,
            sm->timestamp,
            sm->version,
            sm->message_type,
            sm->access_str,
            sm->allow_ip,
            sm->nat_access,
            sm->server_auth,
            sm->client_timeout,
            sm->message,
            sm->digest,
            sm->digest_type,
            sm->enc_pcap_port
    );
}

/***EOF***/
