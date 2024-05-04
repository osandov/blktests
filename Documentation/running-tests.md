# Running Tests

The `./check` script executes tests. `./check` exits with a zero exit status if
all tests passed and non-zero otherwise.

## Test Organization

Tests are split up into various groups, which are the subdirectories of the
`tests` directory. For example, `tests/loop` contains tests for loop devices,
and `tests/block` contains generic block layer tests.

`./check` can execute individual tests or test groups. For example,

```sh
./check loop block/002
```

will run all tests in the `loop` group and the `block/002` test.

## Configuration

Test configuration goes in the `config` file at the top-level directory of the
blktests repository. A different file can be specified with the `-c` command
line option. The `-c` option can be used multiple times; the files will all be
loaded in the order that they are specified on the command line.

Test configuration options can also be set as environment variables. The
configuration file has precedence over environment variables, and command line
options have precedence over the configuration file.

### Test Devices

The `TEST_DEVS` variable is an array of block devices to test on. Tests will be
run on all of these devices where applicable. Note that tests are destructive
and will overwrite any data on these devices.

```sh
TEST_DEVS=(/dev/nvme0n1 /dev/sdb)
```

If `TEST_DEVS` is not defined or is empty, only tests which do not require a
device will be run. If `TEST_DEVS` is defined as a normal variable instead of
an array, it will be converted to an array by splitting on whitespace.

### Excluding Tests


The `EXCLUDE` variable is an array of tests or test groups to exclude. This
corresponds to the `-x` command line option.

```sh
EXCLUDE=(loop block/001)
```

Tests specified explicitly on the command line will always run even if they are
in `EXCLUDE`.

If `EXCLUDE` is defined as a normal variable instead of an array, it will be
converted to an array by splitting on whitespace.

### Quick Runs and Test Timeouts

Many tests can take a long time to run. By setting the `TIMEOUT` variable, you
can limit the runtime of each test to a specific length (in seconds).

```sh
TIMEOUT=60
```

Note that not all tests honor this timeout. You can define the `QUICK_RUN`
variable in addition to `TIMEOUT` to specify that only tests which honor the
timeout or are otherwise "quick" should run. This corresponds to the `-q`
command line option.

```sh
QUICK_RUN=1
TIMEOUT=30
```

### Device-Only Runs

Sometimes it's useful to only run tests which exercise the configured test
devices (e.g., in order to test the device driver itself). This can be done by
passing the `-d` command line option or setting the `DEVICE_ONLY` variable.

```sh
DEVICE_ONLY=1
```

### Zoned Block Device

To run test cases for zoned block devices, set the `RUN_ZONED_TESTS` variable.
When this variable is set and a test case can prepare a virtual device such as
`null_blk` with zoned mode, the test case is executed twice: first in non-zoned
mode and second in zoned mode. The use of the `RUN_ZONED_TESTS` variable
requires that the kernel be compiled with `CONFIG_BLK_DEV_ZONED` enabled.
```sh
RUN_ZONED_TESTS=1
```

### NVMe test parameterizing

The NVMe tests can be additionally parameterized via environment variables.

- NVMET_TRTYPES: 'loop' (default), 'tcp', 'rdma' and 'fc'
  Set up NVME target backends with the specified transport. Multiple transports
  can be listed with separating spaces, e.g., "loop tcp rdma". In this case, the
  tests are repeated to cover all of the transports specified.
  This parameter had an old name 'nvme_trtype'. The old name is still usable,
  but not recommended.
- NVMET_BLKDEV_TYPES: 'device', 'file'
  Set up NVME target backends with the specified block device type. Multiple
  block device types can be listed with separating spaces. In this case, the
  tests are repeated to cover all of the block device types specified. Default
  value is "device file".
- NVME_IMG_SIZE: '1G' (default)
  Run the tests with given image size in bytes. 'm', 'M', 'g' and 'G' postfix
  are supported. This parameter had an old name 'nvme_img_size'. The old name
  is still usable, but not recommended.
- nvme_num_iter: 1000 (default)
  The number of iterations a test should do.

### Running nvme-rdma and SRP tests

These tests will use the siw (soft-iWARP) driver by default. The rdma_rxe
(soft-RoCE) driver is also supported.

```sh
To use the siw driver:
NVMET_TRTYPES=rdma ./check nvme/
./check srp/

To use the rdma_rxe driver:
use_rxe=1 NVMET_TRTYPES=rdma ./check nvme/
use_rxe=1 ./check srp/
```

### Normal user

To run test cases which require normal user privilege, prepare a user and
specify it to the `NORMAL_USER` variable. The test cases are skipped unless a
valid user is specified.

```sh
NORMAL_USER=blktests_user
```

### Custom Setup

The `config` file is really just a bash file that is sourced at the beginning
of the test run, so it can be used to do any special setup you need. For
example, you could configure `PATH` to find an executable you built from
source:

```sh
export PATH="/root/fio:$PATH"
```

Or, if your setup doesn't mount `configfs` automatically (it probably does),
you could mount it:

```sh
if ! findmnt -t configfs /sys/kernel/config > /dev/null; then
	mount -t configfs configfs /sys/kernel/config
fi
```
