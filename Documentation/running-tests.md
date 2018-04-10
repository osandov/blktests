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
blktests repository.

### Test Devices

The `TEST_DEVS` variable is an array of block devices to test on. Tests will be
run on all of these devices where applicable. Note that tests are destructive
and will overwrite any data on these devices.

```sh
TEST_DEVS=(/dev/nvme0n1 /dev/sdb)
```

If `TEST_DEVS` is not defined or is empty, only tests which do not require a
device will be run.

### Excluding Tests


The `EXCLUDE` variable is an array of tests or test groups to exclude. This
corresponds to the `-x` command line option.

```sh
EXCLUDE=(loop block/001)
```

Tests specified explicitly on the command line will always run even if they are
in `EXCLUDE`.

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
