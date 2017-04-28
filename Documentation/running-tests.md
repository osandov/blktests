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

## Test Results

In addition to printing a summary as the tests run, test results are recorded
in the `results` directory. The layout of that directory looks like this:

```
results
|_ nodev
|  |_ log
|  \_ loop
|     |_ 001.full
|     \_ ...
|_ nvme0n1
|  |_ log
|  \_ block
|     |_ 001.full
|     |_ 002.out.bad
|     |_ 002.full
|     |_ 003.dmesg
|     |_ 004.exitstatus
|     \_ ...
\_ sdb1
   |_ log
   \_ block
      \_ ...
```

The `results` directory contains a directory for each device that the tests
were run on, and a directory named `nodev` for tests that don't run on a
specific device. Each of those directories has a file, `log`, formatted like
so:

```
block/001 pass 1
block/002 fail out
block/003 fail dmesg
block/004 fail exit
```

I.e., column 1 is the test name and column 2 is the status (`pass` or `fail`) .
Column 3 depends on the status:

- For `pass`, it is how many seconds the test took to execute.
- For `fail`, it is the reason the test failed.
    - If the reason is `out`, the output didn't match the reference output. In
      this case, `${TEST_NAME}.out.bad` contains the bad output.
    - If the reason is `exit`, the test exited with a non-zero status. In this
      case, `${TEST_NAME}.exitstatus` contains the exit status.
    - If the reason is `dmesg`, there was a warning, oops, etc. in `dmesg`. In
      this case, the messages will be in `${TEST_NAME}.dmesg`.

Finally, if the test logged any verbose output, it is available in
`${TEST_NAME}.full`.
