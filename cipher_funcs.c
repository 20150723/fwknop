/* $Id$
 *****************************************************************************
 *
 * File:    cipher_funcs.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: Cipher functions used by fwknop
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
#include <sys/time.h>

#include "cipher_funcs.h"
#include "digest.h"

#ifndef RAND_FILE
  #define RAND_FILE "/dev/urandom"
#endif

/* Get random data.
*/
void get_random_data(uint8 *data, int len)
{
    FILE           *rfd;
    struct timeval  tv;
    int             i;

    /* Attempt to read seed data from /dev/urandom.  If that does not
     * work, then fall back to a time-based method (less secure, but
     * probably more portable).
    */
    if((rfd = fopen(RAND_FILE, "r")) == NULL)
    {
        /* Seed based on time (current usecs).
        */
        gettimeofday(&tv, NULL);
        srand(tv.tv_usec);

        for(i=0; i<len; i++)
            *(data+i) = rand() % 0xff;
    }
    else
    {
        /* Read seed from /dev/urandom
        */
        fread(data, len, 1, rfd);
        fclose(rfd);
    }
}

/* Function to generate initial salt and initialization vector (iv).
 * This is is done to be compatible with the data produced via
 * the Perl Crypt::CBC module's use of Rijndael.
*/
void salt_and_iv(RIJNDAEL_context *ctx, char *pass, uint8 *data)
{
    uint8   tmp_buf[64]; //--DSS do we limit pw size (pwlen + 8(salt))? 
    uint8   kiv_buf[48];    /* Key and IV buffer */
    uint8   md5_buf[16];    /* Buffer for computed md5 hash */

    int     kiv_len = 0;
    int     plen = strlen(pass);

    /* If we are decrypting, data will contain the salt. Otherwise,
     * for encryption, we generate a random salt.
    */
    if(data != NULL)
    {
        /* Pull the salt from the data
        */
        memcpy(ctx->salt, (data+8), 8);
    }
    else
    {
        /* Generate a random 8-byte salt.
        */
        get_random_data(ctx->salt, 8);
    }

    /* Now generate the key and initialization vector.
     * (again it is the perl Crypt::CBC way)
    */ 
    memcpy(tmp_buf+16, pass, plen);
    memcpy(tmp_buf+16+plen, ctx->salt, 8);

    while(kiv_len < sizeof(kiv_buf))
    {
        if(kiv_len == 0)
            md5(md5_buf, tmp_buf+16, plen+8);
        else
            md5(md5_buf, tmp_buf, 16+plen+8);

        memcpy(tmp_buf, md5_buf, 16);

        memcpy(kiv_buf + kiv_len, md5_buf, 16);

        kiv_len += 16;
    }

    memcpy(ctx->key, kiv_buf,    32);
    memcpy(ctx->iv,  kiv_buf+32, 16);
}

/* Initialization entry point.
*/
void rijndael_init(RIJNDAEL_context *ctx, char *pass, uchar *data)
{

    /* Use ECB mode to be compatible with the Crypt::CBC perl module.
    */
    ctx->mode = MODE_ECB;

    /* Generate the salt and initialization vector.
    */
    salt_and_iv(ctx, pass, data);

    /* Intialize our rinjdael context.
    */
    rijndael_setup(ctx, 32, ctx->key);
}

/* Take a chunk of data, encrypt it in the same way the perl Crypt::CBC
 * module would.
*/
int fko_encrypt(uchar *in, int in_len, char *pass, uchar *out)
{
    RIJNDAEL_context    ctx;
    uint8               plaintext[16];
    uint8               mixtext[16];
    uint8               ciphertext[16];
    int                 i, pad_val;

    uchar              *ondx = out;

    rijndael_init(&ctx, pass, NULL);

    /* Prepend the salt...
    */
    memcpy(ondx, "Salted__", 8);
    ondx+=8;
    memcpy(ondx, ctx.salt, 8);
    ondx+=8;

    /* Now iterate of the input data and encrypt in 16-byte chunks.
    */
    while(in_len)
    {
        for(i=0; i<sizeof(plaintext); i++)
        {
            if(in_len < 1)
                break;

            plaintext[i] = *in++;
            in_len--;
        }

        pad_val = sizeof(plaintext) - i;

        for(; i < sizeof(plaintext); i++)
            plaintext[i] = pad_val;

        for(i=0; i< 16; i++)
            mixtext[i] = plaintext[i] ^ ctx.iv[i];

        block_encrypt(&ctx, mixtext, 16, ciphertext, ctx.iv);

        memcpy(ctx.iv, ciphertext, 16);

        for(i=0; i<sizeof(ciphertext); i++)
            *ondx++ = ciphertext[i];
    }

    return(ondx - out);
}

/* Decrypt the given data.
*/
int fko_decrypt(uchar *in, int in_len, char *pass, uchar *out)
{
    RIJNDAEL_context    ctx;
    uint8               plaintext[16];
    uint8               mixtext[16];
    uint8               ciphertext[16];
    int                 i, pad_val, pad_err = 0;
    uchar              *pad_s;
    uchar              *ondx = out;

    rijndael_init(&ctx, pass, in);

    /* Remove the salt from the input.
    */
    in_len -= 16;
    memmove(in, in+16, in_len);

    while(in_len)
    {
        for(i=0; i<sizeof(ciphertext); i++)
        {
            if(in_len < 1)
                break;

            ciphertext[i] = *in++;
            in_len--;
        }

        block_decrypt(&ctx, ciphertext, 16, mixtext, ctx.iv);

        for(i=0; i<sizeof(ciphertext); i++)
            plaintext[i] = mixtext[i] ^ ctx.iv[i];

        memcpy(ctx.iv, ciphertext, 16);

        for(i=0; i<sizeof(plaintext); i++)
            *ondx++ = plaintext[i];
    }

    /* Find and remove padding.
    */
    pad_val = *(ondx-1);

    if(pad_val >= 0 && pad_val <= 16)
    {
        pad_s = ondx - pad_val;

        for(i=0; i < (ondx-pad_s); i++)
        {
            if(*(pad_s+i) != pad_val)
                pad_err++;
        }
            
        if(pad_err == 0)
            ondx -= pad_val;
    }

    *ondx = '\0';

    return(ondx - out);
}

void hex_dump(uchar *data, int size)
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
