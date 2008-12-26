/* $Id$
 *****************************************************************************
 *
 * File:    fko_mesage.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: Set/Get the spa message (access req/command/etc) based
 *          on the current spa data.
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

/* Set the SPA message type.
*/
int fko_set_spa_message_type(fko_ctx_t *ctx, short msg_type)
{
    /* Must be initialized
    */
    if(!CTX_INITIALIZED(ctx))
        return FKO_ERROR_CTX_NOT_INITIALIZED;

    if(msg_type < 0 || msg_type >= FKO_LAST_MSG_TYPE)
        return(FKO_ERROR_INVALID_DATA);

    ctx->message_type = msg_type;

    ctx->state |= FKO_SPA_MSG_TYPE_MODIFIED;

    return(FKO_SUCCESS);
}

/* Return the SPA message type.
*/
short fko_get_spa_message_type(fko_ctx_t *ctx)
{
    /* Must be initialized
    */
    if(!CTX_INITIALIZED(ctx))
        return FKO_ERROR_CTX_NOT_INITIALIZED;

    return(ctx->message_type);
}

/* Set the SPA MESSAGE data
*/
int fko_set_spa_message(fko_ctx_t *ctx, const char *msg)
{
    /* Context must be initialized.
    */
    if(!CTX_INITIALIZED(ctx))
        return FKO_ERROR_CTX_NOT_INITIALIZED;

    /* Gotta have a valid string.
    */
    if(msg == NULL || strlen(msg) == 0)
        return(FKO_ERROR_INVALID_DATA);

    /* --DSS XXX: Bail out for now.  But consider just
     *            truncating in the future...
    */
    if(strlen(msg) > MAX_SPA_MESSAGE_SIZE)
        return(FKO_ERROR_DATA_TOO_LARGE);

    /* --DSS TODO: ???
     * Do we want to add message type and format checking here
     * or continue to leave it to the implementor?
    */

    /**/

    /* Just in case this is a subsquent call to this function.  We
     * do not want to be leaking memory.
    */
    if(ctx->message != NULL)
        free(ctx->message);

    ctx->message = strdup(msg);

    ctx->state |= FKO_SPA_MSG_MODIFIED;

    if(ctx->message == NULL)
        return(FKO_ERROR_MEMORY_ALLOCATION);

    return(FKO_SUCCESS);
} 

/* Return the SPA message data.
*/
char* fko_get_spa_message(fko_ctx_t *ctx)
{
    /* Must be initialized
    */
    if(!CTX_INITIALIZED(ctx))
        return NULL;

    return(ctx->message);
}

/***EOF***/
