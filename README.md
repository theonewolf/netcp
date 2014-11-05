netcp
=====

`netcp` is a simple program designed to copy a file to a network target.
It is designed to have no external dependencies whatsoever, thus simplifying
cross-platform builds.

Usage
-----

`netcp` works via TCP over both IPv4, IPv6 and can also reference the target
by DNS/host name.

Example invocations:

```bash
netcp test.file 127.0.0.1 1337                  # specify by IPv4 address
netcp test.file localhost 1337                  # specify by host name
netcp test.file localhost.localdomain 1337      # specify by DNS A record
netcp test.file ipv6.localhost 1337             # specify by DNS AAAA record
netcp test.file ::1 1337                        # specify by IPv6 address
```

Compiling
---------

Just run `make`.


License
-------

Open Source under the MIT License (see [LICENSE](LICENSE)).
