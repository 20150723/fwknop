@os_compatibility = (

    {
        'category' => 'Rijndael',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 Ubuntu-12.04',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '97qvmEEOJyCJk6eLpTpqG+7nx4Is+Ruh2ppeaVMkWB/hLpUEE/Znah9RF5JCbB' .
            'lAZNE2O1w83mout+oyWSj4payd0yuWckikoZYjc7tSSgHIFikOhTm9CHi8ERe9' .
            'jLEYw1wvqE2B7Vvz7XyefNILZdHa+Vx5zYM0o',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'legacy_rand_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 Ubuntu-13.04',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '/gNh7EXXyg23dcrYz0g+KaWJWvTwuPuP349YzrwcLVvVSc5RVJdakdx9Qv0xAWe' .
            'GsZJPmv2e1U31SMrdgF+o7/f2qRDH2hwPU8XLKS73rXpAhZKVAF/crt00HDmaH0' .
            'p+hc3ngPtmE/j0PKeUD+GM81YQPO9NdZu4s',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'legacy_rand_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 FreeBSD-8.2',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '9oprfzL5On8uyH5qJ0JQGLGddfGMI8j9YSafjFth2hiJrYG75FqBJNVvnsvetTA' .
            '46kFmbdXHZCUx5iom9jOtpQnMvZJGex65vV4bSFdVwaoJ/ICkiRHbbzSTZo8qmp' .
            'FTLSYWVhTWQddj4j80Ne6GH0h3zXomg9fJU',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'legacy_rand_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5.1 FreeBSD-9.2',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '+2qk0IIjxlblrk+mzHgS65AQQLEhtnYRZBIEzrqyjBY8dqQMCMiCZGSFP4x+tCJ' .
            'y5Fjx+GBM2dqqdBfYfahoDnPWBieljQp5d2awzUxbC1CpLbi3+bMvguPCc3h0gA' .
            '0f9jdWj6MlYXYJikyF/SjeuYxKnCfX2BxKI',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'legacy_rand_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 OpenBSD-4.9',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '+HNnKhqILxDz1IhEeJnWIQF2v/7H7yZ3TZegUeNltMdUP7SYAHxWg8WD2N4LF3g' .
            'dejv3UxC/FRlHgJz4UeRkloFCQQ0tkQLx6MSoCQHKPlNxATKsfLL3UfHpKbhRG8' .
            'a8S9q8lojKxiWuLZU64h5LXjhH7rR7riyds',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'legacy_rand_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5.1 OpenBSD-5.4',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '86P5MMgWXjAsHf4yPv8oRk9wqB0GFA9wp6u857/VX3kVlugK/D1k7BIjeBBKVM' .
            'RQJ1ZouD3lC1xjNb9KkxXN5MojwUtaCVghY3IUqizQysYHPYwVyJ6INNurXXSF' .
            'pWokVgC+ryT78/PbOoAXKTAg+/tcVbrvCiLmQ',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'legacy_rand_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },

    {
        'category' => 'Rijndael',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5.1 OSX-10.9',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '//I+5NoTIET7LtQJ9BGhoVfBnA9vqgQ8SfvHEMOUpxHKgQa5xLFhKK2ScSXQ80K' .
            '3P0/XwJqm6HsAyJrl7eafble0AR5T04PJPFF6ejWAKbTsCh5VYywQ+2W7eBOJuc' .
            '8tjjKuESWqOaodALS9PaxLrVIfm6dvmwtHU',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'legacy_rand_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },

    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 Ubuntu-12.04',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '97UwsN9v6iQxs9/hrSnbRKDF8byw+niI0PDx0AvuhLP2NmAaz4qktRjl1p7lUA' .
            'GBzLi+4o58wJCkHs3QUluHBcMAS6hqIT7qMc/aQlcJzNsQWEfPCkAE0m1zLnjG' .
            'GE3C9u943F+0csrZs+ysKKca+sVBcAKhsTNsMjT9HojVMdk+r9RhJqUa2JJz7f' .
            'ZIZauvBrygBVg0yh6o',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'hmac_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 Ubuntu-13.04',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '/+pXmgSat9BUrGQLUFFELlOSqVAHSzY/BhPC2eP/gYfW5OrLplejpwh7MYoU06G' .
            'PHRx4vqMBYjxzOcrWp0rFC1rrPzP16nFnRUo5P3jxg1FMjMLI48RrSwCcx8G1qk' .
            'mknUjmg4+8maPhus2x7YhoPTMfMZijWKOaMWhX1G0khDqFfGU8GuehpQdwuGdX/' .
            'oGZnheeQyczK4pY',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'hmac_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 FreeBSD-8.2',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '8au7bGF/FL3EwjMVtTQpAtrzW4JvYAlAtbuEHGdxYS0E0vo18t9dpKoYA/hCLMu' .
            'XqFYIH6zINNYUYjplvL9+zFeygIxiT9dm6PIjjossOIRRNOwdwbS8oeBzsckqMn' .
            'BDo3d0K5I4mhwuWVHyTZUg441+kfm7O8TesDhUoy2ftqTGR3+GHi52/NIVctEAp' .
            'WR6NZowCcHElB9E',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'hmac_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5.1 FreeBSD9.2',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '+HtI50Xp3v1TtDw+GBkYCq9cXGC/qgofjPMqAX+uUg0LdQ0nfxCjpJAWoaZX+aC' .
            'oBa9Na64c0JCRxzH9VTDY7oVkK3s+D8jI7pzXAJAE7ffMukCSeKWI3UaubuwsBt' .
            'ku9Mf7Q5XzySHY0QeoQ/3OGd3lSCTLVxNzk/mdIZ94QX/8uOG+qippPa9JyGVp5' .
            'nkKLD+nrVhvEy30',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'hmac_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5 OpenBSD-4.9',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '9LnU3Tlrn7tpcOxviTD5DHlXZ9T0Sj9R92yvm319zB1xzhMYuwB18C4qgScj5q' .
            'Vop4zyz0L1LQYMZ1/O3S2dqafGLIqSwk7j6YiH0ENVafQvuFVFG8ooeBRxFqzw' .
            'YnywR5R9Drrw+hMxpl40HDb1O07xN7WBOSvNgU2vi8MHT7MSZVh02PKRF8aReL' .
            'cQTD2sxRsn5tGfehC8',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'hmac_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5.1 OpenBSD5.4',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '+ju0kG0l/Qv833iaX5HaAALB8Oh6z//u2/y7/dSnUdg00tRufyl8j96r8xKQAX' .
            'lK5yUQejiphOsX9U7ZZC9fD1Ks1LaOYXP8Iz7WcZctByENeN09WCcQAWX8Zj0O' .
            'XSv5C0fNf0RqMCD5Q6OEJuLfm26FDqnsE9jmUSRcRyFcsbP3po2Ru5nlHM/a8o' .
            '1MDoskdw5VbHMIM6lU',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'hmac_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
    {
        'category' => 'Rijndael+HMAC',
        'subcategory' => 'client->server OS compatibility',
        'detail'   => 'v2.5.1 OSX-10.9',
        'function' => \&os_compatibility,
        'no_ip_check' => 1,
        'pkt' =>
            '/yH60IrOqXpcFhwIvPndytkT40DUWwho+yuvVocfRrrOlOY0szD/Xo+veQ+Ubs' .
            '7Y/szJ/HViJRg2LjDff2AeKz1cWMn6Zg/l+C8TZBBc5Tmwc8PIIOXwjrNHwCv3' .
            'lc9tRToeAVdYyQfM87OgczI6OP9SgKoKfKA5ouI9eIxOlncDn+9TkShRy0+5G+' .
            'xi2vuV4KU0DYxTRvV4',
        'server_positive_output_matches' => [qr/with expire time/],
        'fwknopd_cmdline' => "$fwknopdCmd -c $cf{'disable_aging'} -a $cf{'hmac_access'} " .
            "-d $default_digest_file -p $default_pid_file $intf_str",
        'fw_rule_created' => $NEW_RULE_REQUIRED,
        'fw_rule_removed' => $NEW_RULE_REMOVED,
    },
);
