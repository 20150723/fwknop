/*
 *****************************************************************************
 *
 * File:    utils.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: General/Generic functions for the fwknop server.
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
#include "utils.h"

/* Generic hex dump function.
*/
void
hex_dump(const unsigned char *data, const int size)
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

/* Show the fields of the FKO context.
*/
char *
dump_ctx(fko_ctx_t ctx)
{
    static char buf[CTX_DUMP_BUFSIZE];
    char       *ndx;
    int         cp;

    char       *rand_val        = NULL;
    char       *username        = NULL;
    char       *version         = NULL;
    char       *spa_message     = NULL;
    char       *nat_access      = NULL;
    char       *server_auth     = NULL;
    char       *enc_data        = NULL;
    char       *spa_digest      = NULL;
    char       *spa_data        = NULL;

    time_t      timestamp       = 0;
    short       msg_type        = -1;
    short       digest_type     = -1;
    int         client_timeout  = -1;

    /* Should be checking return values, but this is temp code. --DSS
    */
    fko_get_rand_value(ctx, &rand_val);
    fko_get_username(ctx, &username);
    fko_get_timestamp(ctx, &timestamp);
    fko_get_version(ctx, &version);
    fko_get_spa_message_type(ctx, &msg_type);
    fko_get_spa_message(ctx, &spa_message);
    fko_get_spa_nat_access(ctx, &nat_access);
    fko_get_spa_server_auth(ctx, &server_auth);
    fko_get_spa_client_timeout(ctx, &client_timeout);
    fko_get_spa_digest_type(ctx, &digest_type);
    fko_get_encoded_data(ctx, &enc_data);
    fko_get_spa_digest(ctx, &spa_digest);
    fko_get_spa_data(ctx, &spa_data);

    memset(buf, 0x0, CTX_DUMP_BUFSIZE);

    ndx = buf;

    cp = sprintf(ndx, "SPA Field Values:\n=================\n");
    ndx += cp;
    cp = sprintf(ndx, "   Random Value: %s\n", rand_val == NULL ? "<NULL>" : rand_val);
    ndx += cp;
    cp = sprintf(ndx, "       Username: %s\n", username == NULL ? "<NULL>" : username);
    ndx += cp;
    cp = sprintf(ndx, "      Timestamp: %u\n", (unsigned int) timestamp);
    ndx += cp;
    cp = sprintf(ndx, "    FKO Version: %s\n", version == NULL ? "<NULL>" : version);
    ndx += cp;
    cp = sprintf(ndx, "   Message Type: %i\n", msg_type);
    ndx += cp;
    cp = sprintf(ndx, " Message String: %s\n", spa_message == NULL ? "<NULL>" : spa_message);
    ndx += cp;
    cp = sprintf(ndx, "     Nat Access: %s\n", nat_access == NULL ? "<NULL>" : nat_access);
    ndx += cp;
    cp = sprintf(ndx, "    Server Auth: %s\n", server_auth == NULL ? "<NULL>" : server_auth);
    ndx += cp;
    cp = sprintf(ndx, " Client Timeout: %u\n", client_timeout);
    ndx += cp;
    cp = sprintf(ndx, "    Digest Type: %u\n", digest_type);
    ndx += cp;
    cp = sprintf(ndx, "   Encoded Data: %s\n", enc_data == NULL ? "<NULL>" : enc_data);
    ndx += cp;
    cp = sprintf(ndx, "SPA Data Digest: %s\n", spa_digest == NULL ? "<NULL>" : spa_digest);

    return(buf);
}

/* Basic directory checks (stat() and whether the path is actually
 * a directory).
*/
int
is_valid_dir(const char *path)
{
#if HAVE_STAT
    struct stat st;

    /* If we are unable to stat the given dir, then return with error.
    */
    if(stat(path, &st) != 0)
    {
        fprintf(stderr, "[-] unable to stat() directory: %s: %s\n",
            path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(!S_ISDIR(st.st_mode))
        return(0);
#endif /* HAVE_STAT */

    return(1);
}

int
set_file_perms(const char *file)
{
    int res = 0;

    res = chmod(file, S_IRUSR | S_IWUSR);

    if(res != 0)
    {
        fprintf(stderr, "[-] unable to chmod file %s to user read/write: %s\n",
            file, strerror(errno));
    }
    return res;
}

int
verify_file_perms_ownership(const char *file)
{
#if HAVE_STAT
    struct stat st;

    /* Every file that the fwknop client deals with should be owned
     * by the user and permissions set to 600 (user read/write)
    */
    if((stat(file, &st)) != 0)
    {
        fprintf(stderr, "[-] unable to stat() file: %s: %s\n",
            file, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Make sure it is a regular file
    */
    if(S_ISREG(st.st_mode) != 1 && S_ISLNK(st.st_mode) != 1)
    {
        fprintf(stderr,
            "[-] file: %s is not a regular file or symbolic link.\n",
            file
        );
        return 0;
    }

    if((st.st_mode & (S_IRWXU|S_IRWXG|S_IRWXO)) != (S_IRUSR|S_IWUSR))
    {
        fprintf(stderr,
            "[-] file: %s permissions should only be user read/write (0600, -rw-------)\n",
            file
        );
        return 0;
    }

    if(st.st_uid != getuid())
    {
        fprintf(stderr, "[-] file: %s not owned by current effective user id\n",
            file);
        return 0;
    }
#endif

    return 1;
}

/* Determine if a buffer contains only characters from the base64
 * encoding set
*/
int
is_base64(const unsigned char *buf, unsigned short int len)
{
    unsigned short int  i;
    int                 rv = 1;

    for(i=0; i<len; i++)
    {
        if(!(isalnum(buf[i]) || buf[i] == '/' || buf[i] == '+' || buf[i] == '='))
        {
            rv = 0;
            break;
        }
    }

    return rv;
}

/***EOF***/
