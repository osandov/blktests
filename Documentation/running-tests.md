# Running Tests

The `./check` script executes tests. Without any arguments, it executes the
default set of tests. `./check` exits with a zero exit status if all tests
passed and non-zero otherwise.

## Test Organization

Tests are split up into various categories, which are the subdirectories of the
`tests` directory. For example, `tests/loop` contains tests for loop devices,
and `tests/block` contains generic block layer tests.

Tests also belong to one or more groups. Each test category is also a group.
Additionally, there is a group for common functionality like "discard".
Finally, there are a couple of basic groups:

- The `auto` group is the default set of tests. These tests are expected to
  pass reliably.
- The `quick` group is a subset of `auto` comprising tests that complete
  "quickly" (i.e., in ~30 seconds or less on reasonable hardware).
- The `timed` group comprises tests that honor the configured test timeout (see
  below)

`./check` can execute individual tests or test groups, as well as exclude tests
or test groups. See `./check -h`.

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

Note that only tests in the `timed` group honor the timeout.
