# CPU Load Monitoring

A command-line application for monitoring CPU load for every logical CPU core on Linux.

The application is implemented in C++17 and uses Bazel as the build system.

## Features

- Displays CPU load for every detected CPU core.
- Interactive command-line interface.
- Two application states: **Init** and **Run**.
- Configurable periodic CPU load logging to a file.
- Configurable logging interval and output filename.
- Runtime configuration through the interactive menu.
- Logging can be stopped with `q`.
- Menu navigation with nested `children`.
- RAII-based resource management.
- Preallocated storage for the recurring CPU sampling path.

## Requirements

- Linux
- C++17 compiler
- Bazel
- POSIX APIs where applicable

The CPU accounting data is obtained from Linux `/proc/stat`. This is a Linux-specific interface and is intentionally documented as a design decision in [DECISIONS.md](DECISIONS.md).

## Build

From the project root:

```bash
bazel build //:cpu_load_monitor
```

The executable will be available at:

```bash
./bazel-bin/cpu_load_monitor
```

A build script is also provided:

```bash
./build.sh
```

## Run

Run with default configuration:

```bash
./bazel-bin/cpu_load_monitor
```

Default logging configuration:

- Interval: `5` seconds
- File: `cpu_load.txt`

The logging interval and filename can also be provided as command-line arguments:

```bash
./bazel-bin/cpu_load_monitor 5 cpu.log
```

The first argument is the logging interval in seconds.

The second argument is the output filename.

The interval must be a positive integer.

If the command-line arguments are invalid, the application keeps the default configuration.

## Interactive menu

The application starts in the main menu:

```text
CPU Load Monitor

1. Show CPU load
2. Print to file
3. Exit
```

### Show CPU load

Prints the current CPU load for every detected core:

```text
CPU Load:
  CPU0:   5.81%
  CPU1:   5.06%
  CPU2:   5.10%
  CPU3:   4.43%
```

The load is calculated using the difference between two consecutive CPU snapshots.

### Print to file

The logging menu provides:

```text
1. Start logging
2. Change parameters
3. Back
```

**Start logging** periodically writes CPU load to the configured file.

For example, with a 2-second interval:

```bash
./bazel-bin/cpu_load_monitor 2 cpu.log
```

logging produces a new CPU load measurement every 2 seconds.

Press `q` to stop logging and return to the interactive menu.

**Change parameters** allows changing the logging interval and output filename before starting logging.

## CPU load calculation

Linux `/proc/stat` provides cumulative CPU time counters for each CPU.

The application stores two snapshots:

```text
Previous snapshot
       v
    time passes
       v
Current snapshot
       v
     delta
       v
  CPU load (%)
```

The CPU load is calculated from the difference between the two snapshots.

`idle` and `iowait` are treated as idle CPU time.

## Application states

The application has two states required by the task:

```text
Init
 v
Run
```

### Init

Initialization includes:

- Parsing command-line arguments.
- Detecting the number of online CPU cores.
- Allocating CPU snapshot storage.
- Preparing the reusable input buffer.
- Reserving menu navigation capacity.
- Opening `/proc/stat`.
- Reading the initial CPU snapshot.

### Run

Run handles:

- Interactive menu commands.
- CPU load measurement.
- Periodic file logging.
- Runtime parameter changes.

The recurring CPU sampling path uses storage prepared during Init and does not explicitly allocate dynamic memory.

## Project structure

```text
.
+-- BUILD
+-- WORKSPACE
+-- build.sh
+-- start.sh
+-- DECISIONS.md
+-- README.md
L-- src
    +-- main.cpp
    +-- cli
       +-- cli.cpp
       +-- cli.hpp
       L-- menu_item.hpp
    +-- config
       +-- app_config.hpp
       +-- command_line_parser.cpp
       L-- command_line_parser.hpp
    +-- cpu
       +-- cpu_load_monitor.cpp
       +-- cpu_load_monitor.hpp
       L-- cpu_snapshot.hpp
    +-- init
       +-- init.cpp
       L-- init.hpp
    L-- run
        +-- run.cpp
        L-- run.hpp
```

## Design decisions

Implementation decisions and deviations from the specification are documented separately in:

[DECISIONS.md](DECISIONS.md)

This includes the choice of `/proc/stat`, CPU load calculation, Init/Run responsibilities, memory allocation strategy, menu navigation, logging behavior, and other implementation decisions.