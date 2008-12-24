/* $Id$
 *****************************************************************************
 *
 * File:    fko_rand_value.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: Generate a 16-byte random numeric value.
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
#include "fko_common.h"
#include "fko.h"

#ifdef HAVE_SYS_TIME_H
  #include <sys/time.h>
  #ifdef TIME_WITH_SYS_TIME
    #include <time.h>
  #endif
#endif

#define RAND_FILE "/dev/urandom"

/* Set/Generate the SPA data random value string.
*/
int fko_set_rand_value(fko_ctx_t *ctx, const char *new_val)
{
    FILE           *rfd;
    struct timeval  tv;
    unsigned int    seed;
    char            tmp_buf[FKO_RAND_VAL_SIZE+1] = {0};

    /* Context must be initialized.
    */
    if(!CTX_INITIALIZED(ctx))
        return FKO_ERROR_CTX_NOT_INITIALIZED;

    /* If a valid value was given, use it and return happy.
    */
    if(new_val != NULL)
    {
        if(strlen(new_val) != 16)
            return(FKO_ERROR_INVALID_DATA);

        strcpy(ctx->rand_val, new_val);

        return(FKO_SUCCESS);
    }

    /* Attempt to read seed data from /dev/urandom.  If that does not
     * work, then fall back to a time-based method (less secure, but
     * probably more portable).
    */
    if((rfd = fopen(RAND_FILE, "r")) != NULL)
    {
        /* Read seed from /dev/urandom
        */
        fread(&seed, 4, 1, rfd);
        fclose(rfd);
    }
    else
    {
        /* Seed based on time (current usecs).
        */
        gettimeofday(&tv, NULL);

        seed = tv.tv_usec;
    }

    srand(seed);

    sprintf(ctx->rand_val, "%u", rand());
    
    while(strlen(ctx->rand_val) < FKO_RAND_VAL_SIZE)
    {
        sprintf(tmp_buf, "%u", rand());
        strlcat(ctx->rand_val, tmp_buf, FKO_RAND_VAL_SIZE+1);
    }

    return(FKO_SUCCESS);
} 

/* Return the current rand value.
*/
char* fko_get_rand_value(fko_ctx_t *ctx)
{
    /* Must be initialized
    */
    if(!CTX_INITIALIZED(ctx))
        return NULL;

    return(ctx->rand_val);
}

    
/***EOF***/
