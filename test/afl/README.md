
# Fuzzing fwknop With American Fuzzy Lop (AFL)

## Quick Start

To get going with AFL fuzzing against fwknop:

    $ ./compile/afl-compile.sh
    $ ./fuzzing-wrappers/spa-pkts.sh

Fuzzing results will be placed in fuzzing-output/server-conf.out/. For more
information, read on.

## Introduction

The fwknop project supports various fuzzing strategies, and one of the most
important is usage of the 'American Fuzzy Lop' (AFL) fuzzer written by Michal
Zalewski (see: [http://lcamtuf.coredump.cx/afl/]). Because AFL is not designed to
handle encryption schemes (see the README included in the AFL sources for more
information on this), a special --enable-afl-fuzzing command line switch is
available in the fwknop autoconf configure script. This argument allows
encryption and base64 encoding to be bypassed when feeding SPA packet data to
fwknopd via stdin. It is this feature that enables AFL fuzzing. One exception
to this is made when using AFL to fuzz the fwknoprc file that is leveraged by
the fwknop client - in this case, fwknop should not be compiled with
--enable-afl-fuzzing (use the compile/afl-compile-no-enable-arg.sh script).

The top level directory contains enabling scripts in order to make it easy to
fuzz fwknop with AFL. It is assumed that AFL is installed and in your path. The
files are in this directory are organized as follows:

 * *fuzzing-wrappers/*

  Directory that contains wrapper scripts for running AFL against fwknop. All interaction with AFL should be done with these scripts, and they should be executed from the test/afl/ directory, e.g. "./fuzzing-wrappers/client-rc.sh"

 * *test-cases/*

  Directory for ALF test cases used by the wrapper scripts. There are four areas in fwknop that are fuzzed:
    1. SPA packet encoding/decoding
    2. server access.conf parsing
    3. server fwknopd.conf parsing
    4. client fwknoprc file parsing.

 * *compile/*

  Directory for compilation scripts to ensure fwknop is compiled underneath afl-gcc.

 * *fuzzing-output/*

  Results directory that is made underneath an AFL fuzzing cycle.

Here is an example of what fwknopd produces when compiled for AFL support when
a dummy SPA packet is provided in non-encoded/encrypted form via fwknopd's
stdin:

    $ ./fuzzing-wrappers/helpers/fwknopd-stdin-test.sh
    + SPA_PKT=1716411011200157:root:1397329899:2.0.1:1:127.0.0.2,tcp/22:AAAAA
    + LD_LIBRARY_PATH=../../lib/.libs ../../server/.libs/fwknopd -c ../conf/default_fwknopd.conf -a ../conf/default_access.conf -A -f -t
    + echo -n 1716411011200157:root:1397329899:2.0.1:1:127.0.0.2,tcp/22:AAAAA
    Warning: REQUIRE_SOURCE_ADDRESS not enabled for access stanza source: 'ANY'
    SPA Field Values:
    =================
       Random Value: 1716411011200157
           Username: root
          Timestamp: 1397329899
        FKO Version: 2.0.1
       Message Type: 1 (Access msg)
     Message String: 127.0.0.2,tcp/22
         Nat Access: <NULL>
        Server Auth: <NULL>
     Client Timeout: 0
        Digest Type: 3 (SHA256)
          HMAC Type: 0 (None)
    Encryption Type: 1 (Rijndael)
    Encryption Mode: 2 (CBC)
       Encoded Data: 1716411011200157:root:1397329899:2.0.1:1:127.0.0.2,tcp/22
    SPA Data Digest: AAAAA
               HMAC: <NULL>
     Final SPA Data: 200157:root:1397329899:2.0.1:1:127.0.0.2,tcp/22:AAAAA

    SPA packet decode: Success
