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

### Test Timeout

Many tests can take a long time to run. By setting the `TIMEOUT` variable, you
can limit the runtime of each test to a specific length (in seconds).

```sh
TIMEOUT=30
```

Note that not all tests honor this timeout.
