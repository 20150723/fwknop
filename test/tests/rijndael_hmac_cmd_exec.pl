
### need to add a lot more tests to this

@rijndael_hmac_cmd_exec = (
    ### command execution tests
    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client+server',
        'detail'   => 'command execution',
        'function' => \&spa_cmd_exec_cycle,
        'cmdline'  => qq|$fwknopCmd --server-cmd "echo fwknoptest > $cmd_exec_test_file" | .
            "-a $fake_ip -D $loopback_ip --rc-file $cf{'rc_hmac_b64_key'} ".
            "$verbose_str",
        'fwknopd_cmdline'  => "$fwknopdCmd -c $cf{'def'} -a $cf{'hmac_cmd_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $REQUIRE_NO_NEW_RULE,
    },
);
