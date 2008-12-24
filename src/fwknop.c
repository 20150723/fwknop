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
#include <stdio.h>
#include <string.h>
#include "fko.h"

void display_ctx(fko_ctx_t *ctx);
void hex_dump(unsigned char *data, int size);

int main(int argc, char **argv)
{
    fko_ctx_t   ctx;
    int         res;

    /* Intialize the context */
    res = fko_new(&ctx);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_new: %s\n", res, fko_errstr(res));

    /* Set a message string */
    res = fko_set_spa_message(&ctx, "0.0.0.0,access.0");
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_spa_message: %s\n", res, fko_errstr(res));

    /* Set a nat_access string
    res = fko_set_spa_nat_access(&ctx, "192.168.1.5,10616");
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_nat_access: %s\n", res, fko_errstr(res));
    */

    /* Set a server_auth string
    res = fko_set_spa_server_auth(&ctx, "crypt,letmein");
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_spa_server_auth: %s\n", res, fko_errstr(res));
    */

    /* Set a client_timeout string
    res = fko_set_spa_client_timeout(&ctx, 66);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_set_spa_client_timeout: %s\n", res, fko_errstr(res));
    */

    //fko_set_spa_digest_type(&ctx, FKO_DIGEST_SHA1);

    /* Encode the SPA data */
    res = fko_encode_spa_data(&ctx);
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_encode_spa_data: %s\n", res, fko_errstr(res));
    
    /* Encrypt the SPA data */
    res = fko_encrypt_spa_data(&ctx, "BubbaWasHere");
    if(res != FKO_SUCCESS)
        fprintf(stderr, "Error #%i from fko_encrypt_spa_data: %s\n", res, fko_errstr(res));
    
    display_ctx(&ctx);

    //printf("\nHex dump of fko_ctx:\n====================\n");
    //hex_dump((unsigned char*)&ctx, sizeof(fko_ctx_t));

    fko_destroy(&ctx);

    return(0);
} 

void display_ctx(fko_ctx_t *ctx)
{
    printf("\nFKO Context Values:\n===================\n\n");

    if((ctx->state & FKO_CTX_INITIALIZED) == 0)
    {
        printf("*Context not initialized*\n");
        return;
    }

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
        "SPA Data Digest: %s\n"
        "\n   Encoded Data:\n%s\n"
        "\n Encrypted Data:\n%s\n"
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
        (fko_get_spa_digest(ctx) == NULL) ? "<NULL>" : fko_get_spa_digest(ctx),

        (ctx->encoded_msg == NULL) ? "<NULL>" : ctx->encoded_msg,
        (ctx->encrypted_msg == NULL) ? "<NULL>" : ctx->encrypted_msg
    );

}

void hex_dump(unsigned char *data, int size)
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

/***EOF***/
