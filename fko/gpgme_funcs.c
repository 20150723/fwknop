/* $Id$
 *****************************************************************************
 *
 * File:    gpgme_funcs.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: gpgme-related functions for GPG encryptions support in libfko.
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

#if HAVE_LIBGPGME
#include "gpgme_funcs.h"

int
init_gpgme(void)
{
    gpgme_error_t err;

    /* Because the gpgme manual says you should.
    */
    gpgme_check_version(NULL);

    /* Check for OpenPGP support
    */
    err = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        /* GPG engine is not available. */
        return(FKO_ERROR_GPGME_NO_OPENPGP);
    }

    return(FKO_SUCCESS);
}

/* Callback function that supplies the password when gpgme needs it.
*/
gpgme_error_t
passphrase_cb(
  void *pw, const char *uid_hint, const char *passphrase_info,
  int prev_was_bad, int fd)
{
    /* We only need to try once as it is fed by the program
     * (for now --DSS).
    */
    if(prev_was_bad)
        return(GPG_ERR_CANCELED);

    write(fd, (const char*)pw, strlen((const char*)pw));
    write(fd, "\n", 1);

    return 0;
}

int
get_gpg_key(fko_ctx_t fko_ctx, gpgme_key_t *mykey, int signer)
{
    gpgme_error_t err;
    gpgme_ctx_t list_ctx;
    gpgme_key_t key, key2;
    char *name;

    /* Create a gpgme context for the list
    */
    err = gpgme_new(&list_ctx);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        fko_ctx->gpg_err = err;

        if(signer)
            return(FKO_ERROR_GPGME_CONTEXT_SIGNER_KEY);
        else
            return(FKO_ERROR_GPGME_CONTEXT_RECIPIENT_KEY);
    }

    if(signer)
        name = fko_ctx->gpg_signer;
    else
        name = fko_ctx->gpg_recipient;

    err = gpgme_op_keylist_start(list_ctx, name, signer);
    if (err)
    {
        gpgme_release(list_ctx);

        fko_ctx->gpg_err = err;

        if(signer)
            return(FKO_ERROR_GPGME_SIGNER_KEYLIST_START);
        else
            return(FKO_ERROR_GPGME_RECIPIENT_KEYLIST_START);
    }

    /* Grab the first key in the list (we hope it is the only one).
    */
    err = gpgme_op_keylist_next(list_ctx, &key);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        /* Key not found
        */
        gpgme_release(list_ctx);

        fko_ctx->gpg_err = err;

        if(signer)
            return(FKO_ERROR_GPGME_SIGNER_KEY_NOT_FOUND);
        else
            return(FKO_ERROR_GPGME_RECIPIENT_KEY_NOT_FOUND);
    }

    /* We try to get the next key match. If we do, then the name is
     * ambiguous, so we return an error.
    */
    err = gpgme_op_keylist_next(list_ctx, &key2);
    if(gpg_err_code(err) == GPG_ERR_NO_ERROR) /* Note: look for NO error */
    {
        /* Ambiguous specfication of key
        */
        gpgme_key_release(key);
        gpgme_key_release(key2);
        gpgme_release(list_ctx);

        fko_ctx->gpg_err = err;

        if(signer)
            return(FKO_ERROR_GPGME_SIGNER_KEY_AMBIGUOUS);
        else
            return(FKO_ERROR_GPGME_RECIPIENT_KEY_AMBIGUOUS);
    }

    gpgme_op_keylist_end(list_ctx);

    /* --DSS temp for debugging
    fprintf(stderr, "Got Key:\n%s: %s <%s>\n",
            key->subkeys->keyid, key->uids->name, key->uids->email);
    */

    *mykey = fko_ctx->recipient_key = key;

    return(0);
}

/* The main GPG encryption routine for libfko.
*/
int
gpgme_encrypt(fko_ctx_t fko_ctx, unsigned char *indata, size_t in_len, const char *pw, unsigned char **out, size_t *out_len)
{
    char               *tmp_buf;
    int                 res;

    gpgme_ctx_t         gpg_ctx;

    gpgme_error_t       err;
    gpgme_key_t         key[2] = { NULL, NULL };
    gpgme_data_t        plaintext, cipher;

    /* Initialize gpgme
    */
    res = init_gpgme();
    if(res != FKO_SUCCESS)
        return(res);

    /* Create our gpgme context
    */
    err = gpgme_new(&(fko_ctx->gpg_ctx));
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_CONTEXT);
    }

    gpg_ctx = fko_ctx->gpg_ctx;

    /* Initialize the plaintext data (place into gpgme_data object)
    */
    err = gpgme_data_new_from_mem(&plaintext, (char*)indata, in_len, 1);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_PLAINTEXT_DATA_OBJ);
    }

    /* Set protocol
    */
    err = gpgme_set_protocol(gpg_ctx, GPGME_PROTOCOL_OpenPGP);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_data_release(plaintext);
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_SET_PROTOCOL);
    }

    /* Set ascii-armor off (we will be base64-encoding the encrypted data
     * ourselves.
    */
    gpgme_set_armor(gpg_ctx, 0);

    /* Get the signer gpg key
    */
    res = get_gpg_key(fko_ctx, &(fko_ctx->signer_key), 1);
    if(res != FKO_SUCCESS)
    {
        gpgme_data_release(plaintext);
        gpgme_release(gpg_ctx);
        return(res);
    }

    /* Get the recipient gpg key
    */
    res = get_gpg_key(fko_ctx, &(fko_ctx->recipient_key), 0);
    if(res != FKO_SUCCESS)
    {
        gpgme_data_release(plaintext);
        gpgme_release(gpg_ctx);
        return(res);
    }

    key[0] = fko_ctx->recipient_key;

    /* Create the buffer for our encrypted data.
    */
    err = gpgme_data_new(&cipher);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_data_release(plaintext);
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_CIPHER_DATA_OBJ);
    }

    gpgme_signers_clear(gpg_ctx);
    err = gpgme_signers_add(gpg_ctx, fko_ctx->signer_key);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_data_release(plaintext);
        gpgme_data_release(cipher);
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_ADD_SIGNER);
    }

    /* Set the passphrase callback.
    */
    gpgme_set_passphrase_cb(gpg_ctx, passphrase_cb, (void*)pw);

    err = gpgme_op_encrypt_sign(gpg_ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, plaintext, cipher);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_data_release(plaintext);
        gpgme_data_release(cipher);
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        if(gpgme_err_code(err) == GPG_ERR_CANCELED)
            return(FKO_ERROR_GPGME_BAD_PASSPHRASE);

        return(FKO_ERROR_GPGME_ENCRYPT_SIGN);
    }

    /* Done with the plaintext.
    */
    gpgme_data_release(plaintext);

    /* Get the encrypted data and its length from the gpgme data object.
    */
    tmp_buf = gpgme_data_release_and_get_mem(cipher, out_len);

    *out = malloc(*out_len);
    if(*out == NULL)
    {
        res = FKO_ERROR_MEMORY_ALLOCATION;
    }
    else
    {
        memcpy(*out, tmp_buf, *out_len);
        res = FKO_SUCCESS;
    }

    gpgme_free(tmp_buf);
    gpgme_release(gpg_ctx);

    return(res);
}

/* The main GPG encryption routine for libfko.
*/
int
gpgme_decrypt(fko_ctx_t fko_ctx, unsigned char *indata, size_t in_len, const char *pw, unsigned char **out, size_t *out_len)
{
    char                   *tmp_buf;
    int                     res;

    gpgme_ctx_t             gpg_ctx;

    gpgme_error_t           err;
    gpgme_data_t            cipher, plaintext;

    /* Initialize gpgme
    */
    res = init_gpgme();
    if(res != FKO_SUCCESS)
        return(res);

    /* Create our gpgme context
    */
    err = gpgme_new(&(fko_ctx->gpg_ctx));
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_CONTEXT);
    }

    gpg_ctx = fko_ctx->gpg_ctx;

    err = gpgme_data_new(&plaintext);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_PLAINTEXT_DATA_OBJ);
    }

    /* Initialize the cipher data (place into gpgme_data object)
    */
    err = gpgme_data_new_from_mem(&cipher, (char*)indata, in_len, 0);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_data_release(plaintext);
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_CIPHER_DATA_OBJ);
    }

    /* Set the passphrase callback.
    */
    gpgme_set_passphrase_cb(gpg_ctx, passphrase_cb, (void*)pw);

    /* Now decrypt and verify.
    */
    err = gpgme_op_decrypt_verify(gpg_ctx, cipher, plaintext);
    if(gpg_err_code(err) != GPG_ERR_NO_ERROR)
    {
        gpgme_data_release(plaintext);
        gpgme_data_release(cipher);
        gpgme_release(gpg_ctx);

        fko_ctx->gpg_err = err;

        return(FKO_ERROR_GPGME_DECRYPT_FAILED);
    }

    /* Done with the cipher text.
    */
    gpgme_data_release(cipher);

    /* TODO: Do something with these (or not). For now, put them in the
     *       fko context in case we want to check them later.
    */
    fko_ctx->gpg_decrypt_result = gpgme_op_decrypt_result(gpg_ctx);
    fko_ctx->gpg_verify_result  = gpgme_op_verify_result(gpg_ctx);

    /* Get the encrypted data and its length from the gpgme data object.
    */
    tmp_buf = gpgme_data_release_and_get_mem(plaintext, out_len);

    *out = malloc(*out_len);
    if(*out == NULL)
        res = FKO_ERROR_MEMORY_ALLOCATION;
    else
    {
        memcpy(*out, tmp_buf, *out_len);
        res = FKO_SUCCESS;
    }

    gpgme_free(tmp_buf);
    gpgme_release(gpg_ctx);

    return(res);
}

#endif /* HAVE_LIBGPGME */

/***EOF***/
