# blktests

[![Build Status](https://github.com/osandov/blktests/workflows/CI/badge.svg)](https://github.com/osandov/blktests/actions)

blktests is a test framework for the Linux kernel block layer and storage
stack. It is inspired by the [xfstests](https://git.kernel.org/pub/scm/fs/xfs/xfstests-dev.git/)
filesystem testing framework. It was originally written by Omar Sandoval and
[announced in
2017](https://lore.kernel.org/linux-block/20170512184905.GA15267@vader.DHCP.thefacebook.com/).

## Getting Started

The dependencies are minimal, but make sure you have them installed:

- bash (>= 4.2)
- GNU coreutils
- GNU awk
- util-linux
- fio
- gcc
- make

Some tests require the following:

- e2fsprogs and xfsprogs
- multipath-tools (Debian, openSUSE, Arch Linux) or device-mapper-multipath
  (Fedora)
- dmsetup (Debian) or device-mapper (Fedora, openSUSE, Arch Linux)

Build blktests with `make`. Optionally, install it to a known location with
`make install` (`/usr/local/blktests` by default, but this can be changed by
passing `DESTDIR` and/or `prefix`).

Add the list of block devices you want to test on in a file named `config`
(**note:** these tests are potentially destructive):

```sh
TEST_DEVS=(/dev/nvme0n1 /dev/sdb)
```

And as root, run the default set of tests with `./check`.

**Do not** add anything to the `TEST_DEVS` array containing data that you want
to keep.

See [here](Documentation/running-tests.md) for more detailed information on
configuration and running tests.

## Adding Tests

The `./new` script creates a new test from a template. The generated template
contains more detailed documentation.

Pull requests on GitHub and patches to <linux-block@vger.kernel.org> are both
accepted. See [here](CONTRIBUTING.md) for more information on contributing.
