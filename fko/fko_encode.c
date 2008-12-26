/* $Id$
 *****************************************************************************
 *
 * File:    fko_encode.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: Encodes some pieces of the spa data then puts together all of
 *          the necessary pieces to gether to create the single encoded
 *          message string.
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
#include "base64.h"
#include "digest.h"

/* A rough way to make enough space for a base64 encoded version of
 * the given string, encode it, and return it.
*/
int append_b64(char* tbuf, char *str)
{
    int   len = strlen(str);
    char *bs;

    if((bs = malloc(((len/3)*4)+8)) == NULL)
        return(FKO_ERROR_MEMORY_ALLOCATION);

    b64_encode((unsigned char*)str, bs, len);

    /* --DSS XXX: make sure to check here if later decoding
     *            becomes a problem.
    */
    strip_b64_eq(bs);

    strlcat(tbuf, bs, FKO_ENCODE_TMP_BUF_SIZE);

    free(bs);

    return(FKO_SUCCESS);
}

/* Set the SPA encryption type.
*/
int fko_encode_spa_data(fko_ctx_t *ctx)
{
    int     res, offset = 0;
    char    tbuf[FKO_ENCODE_TMP_BUF_SIZE] = {0};

    /* Must be initialized
    */
    if(!CTX_INITIALIZED(ctx))
        return(FKO_ERROR_CTX_NOT_INITIALIZED);

    /* Check prerequisites.
     * --DSS XXX:  Needs review.  Also, we could make this more robust (or
     *             (at leaset expand the error reporting for the missing
     *             data).
    */
    if(  ctx->username == NULL || strlen(ctx->username) == 0
      || ctx->version  == NULL || strlen(ctx->version)  == 0
      || ctx->message  == NULL || strlen(ctx->message)  == 0)
    {
        return(FKO_ERROR_INCOMPLETE_SPA_DATA);
    }

    if(ctx->message_type == FKO_NAT_ACCESS_MSG)
    {
        if(ctx->nat_access == NULL || strlen(ctx->nat_access) == 0)
            return(FKO_ERROR_INCOMPLETE_SPA_DATA);
    }
 
    /* Put it together a piece at a time, starting with the rand val.
    */
    strcpy(tbuf, ctx->rand_val);

    /* Add the base64-encoded username.
    */
    strlcat(tbuf, ":", FKO_ENCODE_TMP_BUF_SIZE);
    if((res = append_b64(tbuf, ctx->username)) != FKO_SUCCESS)
        return(res);
    
    /* Add the timestamp.
    */
    offset = strlen(tbuf);
    sprintf(((char*)tbuf+offset), ":%u:", ctx->timestamp);

    /* Add the version string.
    */
    strlcat(tbuf, ctx->version, FKO_ENCODE_TMP_BUF_SIZE);

    /* Add the message type value.
    */
    offset = strlen(tbuf);
    sprintf(((char*)tbuf+offset), ":%i:", ctx->message_type);
 
    /* Add the base64-encoded SPA message.
    */
    if((res = append_b64(tbuf, ctx->message)) != FKO_SUCCESS)
        return(res);
    
    /** --DSS TODO:  Need to address nat_access, server_auth
    ***        XXX:  and client_timeout message types here
    **/

    /* If encoded_msg is not null, then we assume it needs to
     * be freed before re-assignment.
    */
    if(ctx->encoded_msg != NULL)
        free(ctx->encoded_msg);

    /* Copy our encoded data into the context.
    */
    ctx->encoded_msg = strdup(tbuf);

    if(ctx->encoded_msg == NULL)
        return(FKO_ERROR_MEMORY_ALLOCATION);

    /* At this point we can compute the digest for this SPA data.
    */
    if((res = fko_set_spa_digest(ctx)) != FKO_SUCCESS)
        return(res);

    /* Here we can clear the modified flags on the SPA data fields.
    */
    FKO_CLEAR_SPA_DATA_MODIFIED(ctx);
 
    return(FKO_SUCCESS);
}

/* Return the SPA encryption type.
*/
int fko_decode_spa_data(fko_ctx_t *ctx)
{
    /* Must be initialized
    */
    if(!CTX_INITIALIZED(ctx))
        return(FKO_ERROR_CTX_NOT_INITIALIZED);



    return(FKO_SUCCESS);
}


/***EOF***/
