## Design Decisions

This document describes implementation decisions made for the CPU Load Monitoring application where the task specification did not define the exact behavior.

## 1. CPU load source

**Decision:** Read CPU accounting data from Linux /proc/stat.

**Justification:** The application targets Linux and needs per-core cumulative CPU counters. /proc/stat provides the required counters for each logical CPU (cpu0, cpu1, etc.). CPU load is calculated from differences between consecutive snapshots.

/proc/stat is Linux-specific rather than a POSIX API. POSIX APIs are used for surrounding system operations where applicable, such as sysconf(), open(), read(), lseek(), close(), and poll().

## 2. CPU load calculation

**Decision:** Calculate load from two CPU snapshots.

**Justification:** /proc/stat provides cumulative CPU time counters rather than an instantaneous percentage. The application stores an initial snapshot during Init and compares it with a later snapshot. The percentage is calculated as the proportion of non-idle CPU time during the interval.

idle and iowait are treated as idle time.

## 3. Init and Run states

**Decision:** Separate initialization and runtime behavior into Init and Run.

**Justification:** The task explicitly requires two states. Init performs setup such as CPU core detection, allocation/reservation of storage, opening /proc/stat, and parsing command-line parameters. Run handles interactive commands and CPU monitoring.

## 4. Dynamic memory allocation

**Decision:** Move application-controlled allocations to Init where practical.

**Justification:** CPU snapshot vectors are sized during Init. The reusable input buffer is allocated as part of the monitor object, and menu navigation capacity is reserved before Run. The recurring CPU sampling path therefore does not explicitly allocate heap memory.

Some operations performed only on user request, such as entering a new filename or constructing an output stream for file logging, may use allocations internally. These are not part of the recurring CPU sampling loop.

## 5. Menu navigation

**Decision:** Store menu navigation as a vector of indexes.

**Justification:** The user-facing menu key and the internal position of a menu item are separate concepts. The navigation path stores indexes of selected parent items, allowing the same menu implementation to navigate through nested children.

The maximum expected menu depth is configured with MAX_PATH_SIZE, and the required capacity is reserved before Run.

## 6. Back action

**Decision:** Implement Back as a normal menu item using a callback that calls CLI::back().

**Justification:** Back does not require special handling in the menu data model. The callback removes the last index from the navigation path and returns the user to the parent menu.

## 7. Exit behavior

**Decision:** Exit changes the CLI running state instead of terminating the process directly.

**Justification:** Direct use of std::exit() bypasses normal stack unwinding for automatic local objects. A running-state flag allows show_menu() and run() to return normally, preserving normal C++ object lifetime and RAII behavior.

## 8. Runtime configuration

**Decision:** Command-line arguments provide initial logging parameters, while the user can change them through the menu when logging is inactive.

**Justification:** The task requires the interval and filename to be configurable. Keeping configuration changes outside the active logging loop avoids concurrent modification of configuration and eliminates the need for threads or synchronization primitives.

## 9. Logging behavior

**Decision:** File logging starts explicitly through the interactive menu.

**Justification:** The command-line arguments configure the logging interval and output filename, but the specification does not require logging to start automatically. The application therefore starts logging when the user selects Start logging.

The logger writes CPU load periodically every configured number of seconds and can be stopped by pressing q.

## 10. Logging interval

**Decision:** Validate the interval as a positive integer.

**Justification:** The specification requires an N-second interval but does not define a maximum value. Therefore, the application accepts any positive integer interval. A non-positive or otherwise invalid value is rejected and the default configuration is retained.

## 11. Output file mode

**Decision:** Open the output file in append mode.

**Justification:** Starting logging should not unexpectedly destroy previously collected measurements. New measurements are appended to the existing file.

## 12. Build system

**Decision:** Use Bazel.

**Justification:** Bazel is listed as a nice-to-have requirement. The project uses a Bazel cc_binary target with C++17 compilation settings.

## 13. C++ standard

**Decision:** Use C++17.

**Justification:** The task allows any standard up to C++17. C++17 provides the language and standard-library features needed by the implementation while staying within the requested limit.