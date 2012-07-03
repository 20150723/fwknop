/*
 *****************************************************************************
 *
 * File:    fwknop.c
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: An implementation of an fwknop client.
 *
 * Copyright 2009-2010 Damien Stuart (dstuart@dstuart.org)
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
#include "fwknop.h"
#include "config_init.h"
#include "spa_comm.h"
#include "utils.h"
#include "getpasswd.h"

/* prototypes
*/
static void get_keys(fko_ctx_t ctx, fko_cli_options_t *options,
    char *key, char *hmac_key, const int crypt_op);
static void display_ctx(fko_ctx_t ctx);
static void errmsg(const char *msg, const int err);
static void show_last_command(void);
static void save_args(int argc, char **argv);
static void run_last_args(fko_cli_options_t *options);
static int set_message_type(fko_ctx_t ctx, fko_cli_options_t *options);
static int set_nat_access(fko_ctx_t ctx, fko_cli_options_t *options);
static int get_rand_port(fko_ctx_t ctx);
int resolve_ip_http(fko_cli_options_t *options);
static void clean_exit(fko_ctx_t ctx, fko_cli_options_t *opts,
    unsigned int exit_status);

int
main(int argc, char **argv)
{
    fko_ctx_t           ctx, ctx2;
    int                 res;
    char               *spa_data, *version;
    char                access_buf[MAX_LINE_LEN] = {0};
    char                key[MAX_KEY_LEN+1]       = {0};
    char                hmac_key[MAX_KEY_LEN+1]  = {0};

    fko_cli_options_t   options;

    /* Handle command line
    */
    config_init(&options, argc, argv);

    /* Handle options that don't require a libfko context
    */
    if(options.run_last_command)
        run_last_args(&options);
    else if(options.show_last_command)
        show_last_command();
    else if (!options.no_save_args)
        save_args(argc, argv);

    /* Intialize the context
    */
    res = fko_new(&ctx);
    if(res != FKO_SUCCESS)
    {
        errmsg("fko_new", res);
        return(EXIT_FAILURE);
    }

    /* Generate Rijndael + HMAC keys from /dev/random (base64
     * encoded) and exit.
    */
    if(options.key_gen)
    {
        fko_key_gen(options.key_base64, options.hmac_key_base64);
        printf("KEY_BASE64: %s\nHMAC_KEY_BASE64: %s\n", options.key_base64, options.hmac_key_base64);
        return(EXIT_SUCCESS);
    }

    /* Display version info and exit.
    */
    if(options.version)
    {
        fko_get_version(ctx, &version);

        fprintf(stdout, "fwknop client %s, FKO protocol version %s\n",
            MY_VERSION, version);

        return(EXIT_SUCCESS);
    }

    /* Set client timeout
    */
    if(options.fw_timeout >= 0)
    {
        res = fko_set_spa_client_timeout(ctx, options.fw_timeout);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_spa_client_timeout", res);
            return(EXIT_FAILURE);
        }
    }

    /* Set the SPA packet message type based on command line options
    */
    res = set_message_type(ctx, &options);
    if(res != FKO_SUCCESS)
    {
        errmsg("fko_set_spa_message_type", res);
        return(EXIT_FAILURE);
    }

    /* Adjust the SPA timestamp if necessary
    */
    if(options.time_offset_plus > 0)
    {
        res = fko_set_timestamp(ctx, options.time_offset_plus);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_timestamp", res);
            return(EXIT_FAILURE);
        }
    }
    if(options.time_offset_minus > 0)
    {
        res = fko_set_timestamp(ctx, -options.time_offset_minus);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_timestamp", res);
            return(EXIT_FAILURE);
        }
    }

    if(options.server_command[0] != 0x0)
    {
        /* Set the access message to a command that the server will
         * execute
        */
        snprintf(access_buf, MAX_LINE_LEN, "%s%s%s",
                options.allow_ip_str, ",", options.server_command);
    }
    else
    {
        /* Resolve the client's public facing IP address if requestesd.
         * if this fails, consider it fatal.
        */
        if (options.resolve_ip_http)
            if(resolve_ip_http(&options) < 0)
                return(EXIT_FAILURE);

        /* Set a message string by combining the allow IP and the
         * port/protocol.  The fwknopd server allows no port/protocol
         * to be specified as well, so in this case append the string
         * "none/0" to the allow IP.
        */
        if(options.access_str[0] != 0x0)
        {
            snprintf(access_buf, MAX_LINE_LEN, "%s%s%s",
                    options.allow_ip_str, ",", options.access_str);
        }
        else
        {
            snprintf(access_buf, MAX_LINE_LEN, "%s%s%s",
                    options.allow_ip_str, ",", "none/0");
        }
    }
    res = fko_set_spa_message(ctx, access_buf);
    if(res != FKO_SUCCESS)
    {
        errmsg("fko_set_spa_message", res);
        return(EXIT_FAILURE);
    }

    /* Set NAT access string
    */
    if (options.nat_local || options.nat_access_str[0] != 0x0)
    {
        res = set_nat_access(ctx, &options);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_nat_access_str", res);
            return(EXIT_FAILURE);
        }
    }

    /* Set username
    */
    if(options.spoof_user[0] != 0x0)
    {
        res = fko_set_username(ctx, options.spoof_user);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_username", res);
            return(EXIT_FAILURE);
        }
    }

    /* Set up for using GPG if specified.
    */
    if(options.use_gpg)
    {
        /* If use-gpg-agent was not specified, then remove the GPG_AGENT_INFO
         * ENV variable if it exists.
        */
#ifndef WIN32
        if(!options.use_gpg_agent)
            unsetenv("GPG_AGENT_INFO");
#endif

        res = fko_set_spa_encryption_type(ctx, FKO_ENCRYPTION_GPG);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_spa_encryption_type", res);
            return(EXIT_FAILURE);
        }

        /* If a GPG home dir was specified, set it here.  Note: Setting
         * this has to occur before calling any of the other GPG-related
         * functions.
        */
        if(options.gpg_home_dir != NULL && strlen(options.gpg_home_dir) > 0)
        {
            res = fko_set_gpg_home_dir(ctx, options.gpg_home_dir);
            if(res != FKO_SUCCESS)
            {
                errmsg("fko_set_gpg_home_dir", res);
                return(EXIT_FAILURE);
            }
        }

        res = fko_set_gpg_recipient(ctx, options.gpg_recipient_key);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_gpg_recipient", res);

            if(IS_GPG_ERROR(res))
                fprintf(stderr, "GPG ERR: %s\n", fko_gpg_errstr(ctx));
            return(EXIT_FAILURE);
        }

        if(options.gpg_signer_key != NULL && strlen(options.gpg_signer_key))
        {
            res = fko_set_gpg_signer(ctx, options.gpg_signer_key);
            if(res != FKO_SUCCESS)
            {
                errmsg("fko_set_gpg_signer", res);

                if(IS_GPG_ERROR(res))
                    fprintf(stderr, "GPG ERR: %s\n", fko_gpg_errstr(ctx));

                return(EXIT_FAILURE);
            }
        }

        res = fko_set_spa_encryption_mode(ctx, FKO_ENC_MODE_ASYMMETRIC);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_spa_encryption_mode", res);
            return(EXIT_FAILURE);
        }
    }

    if(options.encryption_mode && !options.use_gpg)
    {
        res = fko_set_spa_encryption_mode(ctx, options.encryption_mode);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_spa_encryption_mode", res);
            return(EXIT_FAILURE);
        }
    }

    /* Set Digest type.
    */
    if(options.digest_type)
    {
        fko_set_spa_digest_type(ctx, options.digest_type);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_spa_digest_type", res);
            return(EXIT_FAILURE);
        }
    }

    /* Acquire the necessary encryption/hmac keys
    */
    get_keys(ctx, &options, key, hmac_key, CRYPT_OP_ENCRYPT);

    /* Finalize the context data (encrypt and encode the SPA data)
    */
    res = fko_spa_data_final(ctx, key, hmac_key);
    if(res != FKO_SUCCESS)
    {
        errmsg("fko_spa_data_final", res);

        if(IS_GPG_ERROR(res))
            fprintf(stderr, "GPG ERR: %s\n", fko_gpg_errstr(ctx));

        clean_exit(ctx, &options, EXIT_FAILURE);
    }

    /* Display the context data.
    */
    if (options.verbose || options.test)
        display_ctx(ctx);

    /* Save packet data payload if requested.
    */
    if (options.save_packet_file[0] != 0x0)
        write_spa_packet_data(ctx, &options);

    if (options.rand_port)
        options.spa_dst_port = get_rand_port(ctx);

    res = send_spa_packet(ctx, &options);
    if(res < 0)
    {
        fprintf(stderr, "send_spa_packet: packet not sent.\n");
        return(EXIT_FAILURE);
    }
    else
    {
        if(options.verbose)
            fprintf(stderr, "send_spa_packet: bytes sent: %i\n", res);
    }

    /* Run through a decode cycle in test mode (--DSS XXX: This test/decode
     * portion should be moved elsewhere).
    */
    if (options.test)
    {
        /************** Decoding now *****************/

        /* Now we create a new context based on data from the first one.
        */
        res = fko_get_spa_data(ctx, &spa_data);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_get_spa_data", res);
            return(EXIT_FAILURE);
        }

        /* If gpg-home-dir is specified, we have to defer decrypting if we
         * use the fko_new_with_data() function because we need to set the
         * gpg home dir after the context is created, but before we attempt
         * to decrypt the data.  Therefore we either pass NULL for the
         * decryption key to fko_new_with_data() or use fko_new() to create
         * an empty context, populate it with the encrypted data, set our
         * options, then decode it.
        */
        res = fko_new_with_data(&ctx2, spa_data, NULL,
            ctx->encryption_mode, hmac_key);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_new_with_data", res);
            return(EXIT_FAILURE);
        }

        res = fko_set_spa_encryption_mode(ctx2, ctx->encryption_mode);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_spa_encryption_mode", res);
            return(EXIT_FAILURE);
        }

        /* See if we are using gpg and if we need to set the GPG home dir.
        */
        if(options.use_gpg)
        {
            if(options.gpg_home_dir != NULL && strlen(options.gpg_home_dir) > 0)
            {
                res = fko_set_gpg_home_dir(ctx2, options.gpg_home_dir);
                if(res != FKO_SUCCESS)
                {
                    errmsg("fko_set_gpg_home_dir", res);
                    return(EXIT_FAILURE);
                }
            }
        }

        get_keys(ctx2, &options, key, hmac_key, CRYPT_OP_DECRYPT);

        /* Verify HMAC first
        */
        if(options.use_hmac)
            res = fko_verify_hmac(ctx2, hmac_key);

        /* Decrypt
        */
        if(options.use_hmac)
        {
            /* check fko_verify_hmac() return value */
        }
        else
            res = fko_decrypt_spa_data(ctx2, key);

        if(res != FKO_SUCCESS)
        {
            errmsg("fko_decrypt_spa_data", res);

            if(IS_GPG_ERROR(res)) {
                /* we most likely could not decrypt the gpg-encrypted data
                 * because we don't have access to the private key associated
                 * with the public key we used for encryption.  Since this is
                 * expected, return 0 instead of an error condition (so calling
                 * programs like the fwknop test suite don't interpret this as
                 * an unrecoverable error), but print the error string for
                 debugging purposes. */
                fprintf(stderr, "GPG ERR: %s\n%s\n", fko_gpg_errstr(ctx2),
                    "No access to recipient private key?\n");
                return(EXIT_SUCCESS);
            }

            return(EXIT_FAILURE);
        }

        printf("\nDump of the Decoded Data\n");
        display_ctx(ctx2);

        fko_destroy(ctx2);
    }

    clean_exit(ctx, &options, EXIT_SUCCESS);

    return(EXIT_SUCCESS);
}

void
free_configs(fko_cli_options_t *opts)
{
    if (opts->resolve_url != NULL)
        free(opts->resolve_url);
}

static int
get_rand_port(fko_ctx_t ctx)
{
    char *rand_val = NULL;
    int   port     = 0;
    int   res      = 0;

    res = fko_get_rand_value(ctx, &rand_val);
    if(res != FKO_SUCCESS)
    {
        errmsg("get_rand_port(), fko_get_rand_value", res);
        exit(EXIT_FAILURE);
    }

    /* Convert to a random value between 1024 and 65535
    */
    port = (MIN_HIGH_PORT + (abs(atoi(rand_val)) % (MAX_PORT - MIN_HIGH_PORT)));

    /* Force libfko to calculate a new random value since we don't want to
     * given anyone a hint (via the port value) about the contents of the
     * encrypted SPA data.
    */
    res = fko_set_rand_value(ctx, NULL);
    if(res != FKO_SUCCESS)
    {
        errmsg("get_rand_port(), fko_get_rand_value", res);
        exit(EXIT_FAILURE);
    }

    return port;
}

/* See if the string is of the format "<ipv4 addr>:<port>",
 */
static int
ipv4_str_has_port(char *str)
{
    int o1, o2, o3, o4, p;

    /* Force the ':' (if any) to a ','
    */
    char *ndx = strchr(str, ':');
    if(ndx != NULL)
        *ndx = ',';

    /* Check format and values.
    */
    if((sscanf(str, "%u.%u.%u.%u,%u", &o1, &o2, &o3, &o4, &p)) == 5
        && o1 >= 0 && o1 <= 255
        && o2 >= 0 && o2 <= 255
        && o3 >= 0 && o3 <= 255
        && o4 >= 0 && o4 <= 255
        && p  >  0 && p  <  65536)
    {
        return 1;
    }

    return 0;
}

/* Set NAT access string
*/
static int
set_nat_access(fko_ctx_t ctx, fko_cli_options_t *options)
{
    char nat_access_buf[MAX_LINE_LEN] = "";
    int nat_port = 0;

    if (options->nat_rand_port)
        nat_port = get_rand_port(ctx);
    else if (options->nat_port)
        nat_port = options->nat_port;
    else
        nat_port = DEFAULT_NAT_PORT;

    if (options->nat_local && options->nat_access_str[0] == 0x0)
    {
        snprintf(nat_access_buf, MAX_LINE_LEN, "%s,%d",
            options->spa_server_str, nat_port);
    }

    if (nat_access_buf[0] == 0x0 && options->nat_access_str[0] != 0x0)
    {
        if (ipv4_str_has_port(options->nat_access_str))
        {
            snprintf(nat_access_buf, MAX_LINE_LEN, "%s",
                options->nat_access_str);
        }
        else
        {
            snprintf(nat_access_buf, MAX_LINE_LEN, "%s,%d",
                options->nat_access_str, nat_port);
        }
    }

    return fko_set_spa_nat_access(ctx, nat_access_buf);
}

static int
get_save_file(char *args_save_file)
{
    char *homedir = NULL;
    int rv = 0;

    homedir = getenv("HOME");

    if (homedir != NULL) {
        snprintf(args_save_file, MAX_PATH_LEN, "%s%s%s",
            homedir, "/", ".fwknop.run");
        rv = 1;
    }

    return rv;
}

/* Show the last command that was executed
*/
static void
show_last_command(void)
{
    char args_save_file[MAX_PATH_LEN];
    char args_str[MAX_LINE_LEN] = "";
    FILE *args_file_ptr = NULL;

#ifdef WIN32
    /* Not sure what the right thing is here on Win32, just exit
     * for now.
    */
    fprintf(stderr, "--show-last not implemented on Win32 yet.");
    exit(EXIT_FAILURE);
#endif

    if (get_save_file(args_save_file)) {
        if ((args_file_ptr = fopen(args_save_file, "r")) == NULL) {
            fprintf(stderr, "Could not open args file: %s\n",
                args_save_file);
            exit(EXIT_FAILURE);
        }
        if ((fgets(args_str, MAX_LINE_LEN, args_file_ptr)) != NULL) {
            printf("Last fwknop client command line: %s", args_str);
        } else {
            printf("Could not read line from file: %s\n", args_save_file);
        }
        fclose(args_file_ptr);
    }

    exit(EXIT_SUCCESS);
}

/* Get the command line arguments from the previous invocation
*/
static void
run_last_args(fko_cli_options_t *options)
{
    FILE           *args_file_ptr = NULL;

    int             current_arg_ctr = 0;
    int             argc_new = 0;
    int             i = 0;

    char            args_save_file[MAX_PATH_LEN] = {0};
    char            args_str[MAX_LINE_LEN] = {0};
    char            arg_tmp[MAX_LINE_LEN]  = {0};
    char           *argv_new[200];  /* should be way more than enough */


#ifdef WIN32
    /* Not sure what the right thing is here on Win32, just return
     * for now.
    */
    return;
#endif

    if (get_save_file(args_save_file))
    {
        if ((args_file_ptr = fopen(args_save_file, "r")) == NULL)
        {
            fprintf(stderr, "Could not open args file: %s\n",
                args_save_file);
            exit(EXIT_FAILURE);
        }
        if ((fgets(args_str, MAX_LINE_LEN, args_file_ptr)) != NULL)
        {
            args_str[MAX_LINE_LEN-1] = '\0';
            if (options->verbose)
                printf("Executing: %s\n", args_str);
            for (i=0; i < (int)strlen(args_str); i++)
            {
                if (!isspace(args_str[i]))
                {
                    arg_tmp[current_arg_ctr] = args_str[i];
                    current_arg_ctr++;
                }
                else
                {
                    arg_tmp[current_arg_ctr] = '\0';
                    argv_new[argc_new] = malloc(strlen(arg_tmp)+1);
                    if (argv_new[argc_new] == NULL)
                    {
                        fprintf(stderr, "malloc failure for cmd line arg.\n");
                        exit(EXIT_FAILURE);
                    }
                    strlcpy(argv_new[argc_new], arg_tmp, strlen(arg_tmp)+1);
                    current_arg_ctr = 0;
                    argc_new++;
                }
            }
        }
        fclose(args_file_ptr);

        /* Reset the options index so we can run through them again.
        */
        optind = 0;

        config_init(options, argc_new, argv_new);
    }

    return;
}

/* Save our command line arguments
*/
static void
save_args(int argc, char **argv)
{
    char args_save_file[MAX_PATH_LEN];
    char args_str[MAX_LINE_LEN] = "";
    FILE *args_file_ptr = NULL;
    int i = 0, args_str_len = 0;

#ifdef WIN32
    /* Not sure what the right thing is here on Win32, just return
     * for now.
    */
    return;
#endif


    if (get_save_file(args_save_file)) {
        if ((args_file_ptr = fopen(args_save_file, "w")) == NULL) {
            fprintf(stderr, "Could not open args file: %s\n",
                args_save_file);
            exit(EXIT_FAILURE);
        }
        for (i=0; i < argc; i++) {
            args_str_len += strlen(argv[i]);
            if (args_str_len >= MAX_PATH_LEN) {
                fprintf(stderr, "argument string too long, exiting.\n");
                exit(EXIT_FAILURE);
            }
            strlcat(args_str, argv[i], MAX_PATH_LEN);
            strlcat(args_str, " ", MAX_PATH_LEN);
        }
        fprintf(args_file_ptr, "%s\n", args_str);
        fclose(args_file_ptr);
    }
    return;
}

/* Set the SPA packet message type
*/
static int
set_message_type(fko_ctx_t ctx, fko_cli_options_t *options)
{
    short message_type;

    if(options->server_command[0] != 0x0)
    {
        message_type = FKO_COMMAND_MSG;
    }
    else if(options->nat_access_str[0] != 0x0)
    {
        if (options->nat_local)
        {
            if (options->fw_timeout >= 0)
                message_type = FKO_CLIENT_TIMEOUT_LOCAL_NAT_ACCESS_MSG;
            else
                message_type = FKO_LOCAL_NAT_ACCESS_MSG;
        }
        else
        {
            if (options->fw_timeout >= 0)
                message_type = FKO_CLIENT_TIMEOUT_NAT_ACCESS_MSG;
            else
                message_type = FKO_NAT_ACCESS_MSG;
        }
    }
    else
    {
        if (options->fw_timeout >= 0)
            message_type = FKO_CLIENT_TIMEOUT_ACCESS_MSG;
        else
            message_type = FKO_ACCESS_MSG;
    }

    return fko_set_spa_message_type(ctx, message_type);
}

/* Prompt for and receive a user password.
*/
static void
get_keys(fko_ctx_t ctx, fko_cli_options_t *options,
    char *key, char *hmac_key, const int crypt_op)
{
    int use_hmac = 0, res = 0;

    memset(key, 0x0, MAX_KEY_LEN+1);
    memset(hmac_key, 0x0, MAX_KEY_LEN+1);

    /* First of all if we are using GPG and GPG_AGENT
     * then there is no password to return.
    */
    if(options->use_gpg
      && (options->use_gpg_agent
           || (crypt_op == CRYPT_OP_ENCRYPT && options->gpg_signer_key == NULL)))
        return;

    if (options->have_key)
        strlcpy(key, options->key, MAX_KEY_LEN+1);
    else if (options->have_base64_key)
    {
        fko_base64_decode(options->key_base64, (unsigned char *) options->key);
        memcpy(key, options->key, RIJNDAEL_MAX_KEYSIZE);
    }
    else
    {
        /* If --get-key file was specified grab the key/password from it.
        */
        if (options->get_key_file[0] != 0x0)
        {
            strlcpy(key, getpasswd_file(options->get_key_file,
                options->spa_server_str), MAX_KEY_LEN+1);
        }
        else if (options->use_gpg)
        {
            if(crypt_op == CRYPT_OP_DECRYPT)
                strlcpy(key, getpasswd("Enter passphrase for secret key: "),
                    MAX_KEY_LEN+1);
            else if(options->gpg_signer_key && strlen(options->gpg_signer_key))
                strlcpy(key, getpasswd("Enter passphrase for signing: "),
                    MAX_KEY_LEN+1);
        }
        else
        {
            if(crypt_op == CRYPT_OP_ENCRYPT)
                strlcpy(key, getpasswd("Enter encryption key: "),
                    MAX_KEY_LEN+1);
            else if(crypt_op == CRYPT_OP_DECRYPT)
                strlcpy(key, getpasswd("Enter decryption key: "),
                    MAX_KEY_LEN+1);
            else
                strlcpy(key, getpasswd("Enter key: "),
                    MAX_KEY_LEN+1);
        }
    }


    if (options->have_hmac_key)
    {
        strlcpy(hmac_key, options->hmac_key, MAX_KEY_LEN+1);
        use_hmac = 1;
    }
    else if (options->have_hmac_base64_key)
    {
        fko_base64_decode(options->hmac_key_base64, (unsigned char *) options->hmac_key);
        memcpy(hmac_key, options->hmac_key, SHA256_BLOCK_LENGTH);
        use_hmac = 1;
    }
    else if (options->use_hmac)
    {
        /* If --get-key file was specified grab the key/password from it.
        */
#if 0
        if (options->get_key_file[0] != 0x0)
        {
            key = getpasswd_file(options->get_key_file, options->spa_server_str);
        }
        else
        {
#endif
        strlcpy(hmac_key, getpasswd("Enter HMAC key: "), MAX_KEY_LEN+1);
        use_hmac = 1;
    }

    if (use_hmac)
    {
        res = fko_set_hmac_mode(ctx, FKO_HMAC_SHA256);
        if(res != FKO_SUCCESS)
        {
            errmsg("fko_set_hmac_mode", res);
            exit(EXIT_FAILURE);
        }
    }

    return;
}

/* Display an FKO error message.
*/
void
errmsg(const char *msg, const int err) {
    fprintf(stderr, "%s: %s: Error %i - %s\n",
        MY_NAME, msg, err, fko_errstr(err));
}

/* free up memory and exist
*/
static void
clean_exit(fko_ctx_t ctx, fko_cli_options_t *opts, unsigned int exit_status)
{
    free_configs(opts);
    fko_destroy(ctx);
    exit(exit_status);
}

/* Show the fields of the FKO context.
*/
static void
display_ctx(fko_ctx_t ctx)
{
    char       *rand_val        = NULL;
    char       *username        = NULL;
    char       *version         = NULL;
    char       *spa_message     = NULL;
    char       *nat_access      = NULL;
    char       *server_auth     = NULL;
    char       *enc_data        = NULL;
    char       *hmac_data       = NULL;
    char       *spa_digest      = NULL;
    char       *spa_data        = NULL;

    time_t      timestamp       = 0;
    short       msg_type        = -1;
    short       digest_type     = -1;
    int         encryption_mode = -1;
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
    fko_get_spa_encryption_mode(ctx, &encryption_mode);
    fko_get_encoded_data(ctx, &enc_data);
    fko_get_hmac_data(ctx, &hmac_data);
    fko_get_spa_digest(ctx, &spa_digest);
    fko_get_spa_data(ctx, &spa_data);

    printf("\nFKO Field Values:\n=================\n\n");
    printf("   Random Value: %s\n", rand_val == NULL ? "<NULL>" : rand_val);
    printf("       Username: %s\n", username == NULL ? "<NULL>" : username);
    printf("      Timestamp: %u\n", (unsigned int) timestamp);
    printf("    FKO Version: %s\n", version == NULL ? "<NULL>" : version);
    printf("   Message Type: %i\n", msg_type);
    printf(" Message String: %s\n", spa_message == NULL ? "<NULL>" : spa_message);
    printf("     Nat Access: %s\n", nat_access == NULL ? "<NULL>" : nat_access);
    printf("    Server Auth: %s\n", server_auth == NULL ? "<NULL>" : server_auth);
    printf(" Client Timeout: %u\n", client_timeout);
    printf("    Digest Type: %d\n", digest_type);
    printf("Encryption Mode: %d\n", encryption_mode);
    printf("\n   Encoded Data: %s\n", enc_data == NULL ? "<NULL>" : enc_data);
    printf("SPA Data Digest: %s\n", spa_digest == NULL ? "<NULL>" : spa_digest);
    printf("    HMAC-SHA256: %s\n", hmac_data == NULL ? "<NULL>" : hmac_data);

    if (enc_data != NULL && spa_digest != NULL)
        printf("      Plaintext: %s:%s\n", enc_data, spa_digest);

    printf("\nFinal Packed/Encrypted/Encoded Data:\n\n%s\n\n", spa_data);
}

/***EOF***/
