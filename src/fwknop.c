/* $Id$
 *****************************************************************************
 *
 * File:    fko_test.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: Temp test program for libfwknop
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

/* includes */
#include "fwknop.h"

int
main(int argc, char **argv)
{
    fko_ctx_t   ctx, ctx2;
    int         res;
    cmdl_opts    options;

    /* Handle command line
    */
    process_cmd_line(&options, argc, argv);

    /* Intialize the context
    */
    res = fko_new(&ctx);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_new: %s\n", res, fko_errstr(res));

    if (options.version) {
        fprintf(stdout, "[+] fwknop-%s\n", fko_version(ctx));
        exit(0);
    }

    /* Set message type
    res = fko_set_spa_message_type(ctx, FKO_ACCESS_MSG);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_spa_message_type: %s\n", res, fko_errstr(res));
    */

    /* Set a message string
    */
    res = fko_set_spa_message(ctx, "0.0.0.0,tcp/22");
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_spa_message: %s\n", res, fko_errstr(res));

    /* Set Digest type.
    fko_set_spa_digest_type(ctx, FKO_DIGEST_SHA1);
    */

    /* Set net access string.
    res = fko_set_spa_nat_access(ctx, "192.168.1.2,22");
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_spa_nat_access: %s\n", res, fko_errstr(res));
    */

    /* Set client timeout value
    fko_set_spa_client_timeout(ctx, 120);
    */

    /* Set a serer auth string.
    res = fko_set_spa_server_auth(ctx, "crypt,SomePW");
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_spa_server_auth: %s\n", res, fko_errstr(res));
    */

    /* Finalize the context data (encrypt and encode the SPA data)
    */
    res = fko_spa_data_final(ctx, FKO_PW);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_spa_data_final: %s\n", res, fko_errstr(res));

    /* Display the context data.
    */
    if (! options.quiet)
        display_ctx(ctx);

    /* Send the SPA data across the wire with a protocol/port specified on the
    * command line (default is UDP/62201) */
    if (! options.test)
        send_spa_packet(ctx, &options);

    /************** Decoding now *****************/

    /* Now we create a new context based on data from the first one.
    */
    //res = fko_new_with_data(&ctx2, ctx->encrypted_msg, FKO_PW);
    res = fko_new_with_data(&ctx2, fko_get_spa_data(ctx), FKO_PW);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_new_with_data: %s\n", res, fko_errstr(res));

    /* Simply call fko_decrypt_spa_data to do all decryption, decoding,
     * parsing, and populating the context.
    res = fko_decrypt_spa_data(ctx2, FKO_PW);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_decrypt_spa_data: %s\n", res, fko_errstr(res));
    */

    if (! options.quiet) {
        printf("\nDump of the Decoded Data\n");
        display_ctx(ctx2);
    }

    fko_destroy(ctx);
    fko_destroy(ctx2);

    return(0);
}

static int
send_spa_packet(fko_ctx_t ctx, cmdl_opts *options)
{
    int rv = 0;
    struct sockaddr_in saddr, daddr;

    /* initialize to zeros
    */
    memset(&saddr, 0, sizeof(saddr));
    memset(&daddr, 0, sizeof(daddr));

    saddr.sin_family = AF_INET;
    daddr.sin_family = AF_INET;

    /* set source address and port
    */
    saddr.sin_port = INADDR_ANY;  /* default */
    if (options->src_port)
        saddr.sin_port = htons(options->src_port);

    saddr.sin_addr.s_addr = INADDR_ANY;  /* default */
    if (options->spoof_ip_src_str[0] != 0x00)
        saddr.sin_addr.s_addr = inet_addr(options->spoof_ip_src_str);

    /* set destination address and port */
    daddr.sin_port = htons(options->port);
    daddr.sin_addr.s_addr = inet_addr(options->spa_server_ip_str);

    if (options->proto == IPPROTO_UDP)
        rv = send_spa_packet_udp(ctx, &saddr, &daddr, options);
    else if (options->proto == IPPROTO_TCP)
        rv = send_spa_packet_tcp(ctx, &saddr, &daddr, options);
    else if (options->proto == IPPROTO_ICMP)
        rv = send_spa_packet_icmp(ctx, options);

    return rv;
}

static int
send_spa_packet_udp(fko_ctx_t ctx, struct sockaddr_in *saddr,
    struct sockaddr_in *daddr, cmdl_opts *options)
{
    int rv = 0;
    int sock = 0;

    /* create the socket
    */
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        fprintf(stderr, "[*] Could not create UDP socket.\n");
        exit(1);
    }

    sendto(sock, fko_get_spa_data(ctx), strlen(fko_get_spa_data(ctx)),
            0, (struct sockaddr *)daddr, sizeof(*daddr));

    return rv;
}

static int
send_spa_packet_tcp(fko_ctx_t ctx, struct sockaddr_in *saddr,
    struct sockaddr_in *daddr, cmdl_opts *options)
{
    int rv;
    return rv;
}

static int
send_spa_packet_icmp(fko_ctx_t ctx, cmdl_opts *options)
{
    int rv;
    return rv;
}

static void
display_ctx(fko_ctx_t ctx)
{
    printf("\nFKO Context Values:\n===================\n\n");

    printf(
        "   Random Value: %s\n"
        "       Username: %s\n"
        "      Timestamp: %u\n"
        "    FKO Version: %s\n"
        "   Message Type: %i\n"
        " Message String: %s\n"
        "     Nat Access: %s\n"
        "    Server Auth: %s\n"
        " Client Timeout: %u\n"
        "    Digest Type: %u\n"
        "\n   Encoded Data: %s\n"
        "\nSPA Data Digest: %s\n"
        "\nFinal Packed/Encrypted/Encoded Data:\n\n%s\n\n"
        ,
        fko_get_rand_value(ctx),
        (fko_get_username(ctx) == NULL) ? "<NULL>" : fko_get_username(ctx),
        fko_get_timestamp(ctx),
        fko_version(ctx),
        fko_get_spa_message_type(ctx),
        (fko_get_spa_message(ctx) == NULL) ? "<NULL>" : fko_get_spa_message(ctx),
        (fko_get_spa_nat_access(ctx) == NULL) ? "<NULL>" : fko_get_spa_nat_access(ctx),
        (fko_get_spa_server_auth(ctx) == NULL) ? "<NULL>" : fko_get_spa_server_auth(ctx),
        fko_get_spa_client_timeout(ctx),
        fko_get_spa_digest_type(ctx),
        (fko_get_encoded_data(ctx) == NULL) ? "<NULL>" : fko_get_encoded_data(ctx),
        (fko_get_spa_digest(ctx) == NULL) ? "<NULL>" : fko_get_spa_digest(ctx),

        (fko_get_spa_data(ctx) == NULL) ? "<NULL>" : fko_get_spa_data(ctx)
    );

}

static void
hex_dump(unsigned char *data, int size)
{
    int ln, i, j = 0;
    char ascii_str[17] = {0};

    for(i=0; i<size; i++)
    {
        if((i % 16) == 0)
        {
            printf(" %s\n  0x%.4x:  ", ascii_str, i);
            memset(ascii_str, 0x0, 17);
            j = 0;
        }

        printf("%.2x ", data[i]);

        ascii_str[j++] = (data[i] < 0x20 || data[i] > 0x7e) ? '.' : data[i];

        if(j == 8)
            printf(" ");
    }

    /* Remainder...
    */
    ln = strlen(ascii_str);
    if(ln > 0)
    {
        for(i=0; i < 16-ln; i++)
            printf("   ");

        printf(" %s\n\n", ascii_str);
    }
}

static void process_cmd_line(cmdl_opts *options, int argc, char **argv)
{
    int getopt_c = 0;
    int opt_index = 0;

    memset(options, 0x00, sizeof(cmdl_opts));

    /* establish a few defaults such as UDP/62201 for sending the SPA
    * packet (can be changed with --Server-proto/--Server-port) */
    options->proto = FKO_DEFAULT_PROTO;
    options->port  = FKO_DEFAULT_PORT;
    options->spa_server_ip_str[0] = 0x00;
    options->spoof_ip_src_str[0] = 0x00;

    while (1) {
        opt_index = 0;
        static struct option long_options[] = {
            {"Destination", CMDL_HAS_ARG, NULL, 'D'},
            {"Server-port", CMDL_HAS_ARG, NULL, 'p'},
            {"Server-proto", CMDL_HAS_ARG, NULL, 'P'},
            {"Source-port", CMDL_HAS_ARG, NULL, 'S'},
            {"Spoof-src", CMDL_HAS_ARG, NULL, 'Q'},
            {"quiet", CMDL_NO_ARG, NULL, 'q'},
            {"Test", CMDL_NO_ARG, NULL, 'T'},
            {"verbose", CMDL_NO_ARG, NULL, 'v'},
            {"Version", CMDL_NO_ARG, NULL, 'V'},
            {"help", CMDL_NO_ARG, NULL, 'h'},
            {0, 0, 0, 0}
        };
        getopt_c = getopt_long(argc, argv, "D:S:Q:p:P:TqhvV",
                long_options, &opt_index);
        if (getopt_c == -1)
            break;

        switch (getopt_c) {
            case 'D':
                strlcpy(options->spa_server_ip_str, optarg, MAX_IP_STR_LEN);
                break;
            case 'Q':
                strlcpy(options->spoof_ip_src_str, optarg, MAX_IP_STR_LEN);
                break;
            case 'p':
                options->port = atoi(optarg);
                if (options->port < 0 || options->port > 65535) {
                    fprintf(stderr, "[*] Unrecognized port: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'P':
                if (strncmp(optarg, "udp", strlen("udp")) == 0)
                    options->proto = IPPROTO_UDP;
                else if (strncmp(optarg, "tcp", strlen("tcp")) == 0)
                    options->proto = IPPROTO_TCP;
                else if (strncmp(optarg, "icmp", strlen("icmp")) == 0)
                    options->proto = IPPROTO_ICMP;
                else {
                    fprintf(stderr, "[*] Unrecognized protocol: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'S':
                options->src_port = atoi(optarg);
                if (options->port < 0 || options->port > 65535) {
                    fprintf(stderr, "[*] Unrecognized port: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'q':
                options->quiet = 1;
                break;
            case 'T':
                options->test = 1;
                break;
            case 'v':
                options->verbose = 1;
                break;
            case 'V':
                options->version = 1;
                break;
            case 'h':
                usage();
                exit(0);
           default:
               printf("?? getopt_long returned character code 0%o ??\n",
                    getopt_c);
        }
    }

    /* perform some additional options validation
    */
    validate_options(options);

    return;
}

static
void validate_options(cmdl_opts *options)
{
    if (!options->test && options->spa_server_ip_str[0] == 0x00) {
        fprintf(stderr,
            "[*] Must use --Destination unless --Test mode is used\n");
        exit(1);
    }
    return;
}

static
void usage(void)
{
    fprintf(stdout,
"fwknop; Single Packet Authorization client\n"
"\n"
"Usage: fwknop -A <port list> [-s|-R|-a] -D <spa_server> [options]\n"
"\n"
"Options:\n"
"    -A, --Access  <port list>  - Provide a list of ports/protocols to open\n"
"                                 on the server. The format is\n"
    );
    return;
}

/***EOF***/
