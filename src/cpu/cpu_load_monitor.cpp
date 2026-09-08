#include "cpu_load_monitor.hpp"

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

bool CpuLoadMonitor::init() {
  long core_count = sysconf(_SC_NPROCESSORS_ONLN);

  if (core_count <= 0) {
    std::cerr << "Failed to get CPU core count." << std::endl;
    return false;
  }

  previous_snapshots.resize(core_count);
  current_snapshots.resize(core_count);

  stat_fd = open("/proc/stat", O_RDONLY);

  if (stat_fd == -1) {
    std::cerr << "Failed to open /proc/stat." << std::endl;
    return false;
  }

  if (!read_snapshots(previous_snapshots)) {
    std::cerr << "Failed to read initial CPU snapshot." << std::endl;
    return false;
  }

  return true;
}

CpuLoadMonitor::~CpuLoadMonitor() {
  if (stat_fd != -1) {
    close(stat_fd);
  }
}

void CpuLoadMonitor::show_load() {
  if (!read_snapshots(current_snapshots)) {
    std::cerr << "Failed to read CPU snapshot." << std::endl;
    return;
  }

  std::cout << std::endl;
  std::cout << "CPU Load:" << std::endl;

  for (std::size_t i = 0; i < previous_snapshots.size(); ++i) {
    const CpuSnapshot& previous = previous_snapshots[i];
    const CpuSnapshot& current = current_snapshots[i];

    unsigned long long previous_idle = previous.idle + previous.iowait;

    unsigned long long current_idle = current.idle + current.iowait;

    unsigned long long previous_total =
        previous.user + previous.nice + previous.system + previous.idle +
        previous.iowait + previous.irq + previous.softirq + previous.steal;

    unsigned long long current_total =
        current.user + current.nice + current.system + current.idle +
        current.iowait + current.irq + current.softirq + current.steal;

    unsigned long long total_delta = current_total - previous_total;

    unsigned long long idle_delta = current_idle - previous_idle;

    double load = 0.0;

    if (total_delta > 0) {
      load = 100.0 * (static_cast<double>(total_delta - idle_delta) /
                      static_cast<double>(total_delta));
    }

    std::cout << "  CPU" << i << ": " << std::fixed << std::setprecision(2)
              << std::setw(6) << load << "%" << std::endl;
  }

  std::cout << std::endl;

  previous_snapshots = current_snapshots;
}

bool CpuLoadMonitor::read_snapshots(std::vector<CpuSnapshot>& snapshots) {
  if (lseek(stat_fd, 0, SEEK_SET) == -1) {
    return false;
  }

  std::size_t cores_read = 0;
  std::size_t buffer_size = 0;

  while (cores_read < snapshots.size()) {
    ssize_t bytes_read =
        read(stat_fd, buffer + buffer_size, BUFFER_SIZE - buffer_size - 1);

    if (bytes_read < 0) {
      return false;
    }

    if (bytes_read == 0) {
      break;
    }

    buffer_size += static_cast<std::size_t>(bytes_read);
    buffer[buffer_size] = '\0';

    char* line = buffer;

    while (true) {
      char* newline = nullptr;

      for (char* current = line; *current != '\0'; ++current) {
        if (*current == '\n') {
          newline = current;
          break;
        }
      }

      if (newline == nullptr) {
        break;
      }

      *newline = '\0';

      if (line[0] == 'c' && line[1] == 'p' && line[2] == 'u' &&
          line[3] >= '0' && line[3] <= '9') {
        std::size_t core_index = 0;
        char* value = line + 3;

        while (*value >= '0' && *value <= '9') {
          core_index = core_index * 10 + static_cast<std::size_t>(*value - '0');

          ++value;
        }

        if (core_index < snapshots.size()) {
          CpuSnapshot& snapshot = snapshots[core_index];

          int parsed = sscanf(value,
                              "%llu %llu %llu %llu %llu "
                              "%llu %llu %llu %llu %llu",
                              &snapshot.user, &snapshot.nice, &snapshot.system,
                              &snapshot.idle, &snapshot.iowait, &snapshot.irq,
                              &snapshot.softirq, &snapshot.steal,
                              &snapshot.guest, &snapshot.guest_nice);

          if (parsed != 10) {
            return false;
          }

          ++cores_read;
        }
      }

      line = newline + 1;
    }

    if (line != buffer) {
      std::size_t remaining =
          buffer_size - static_cast<std::size_t>(line - buffer);

      for (std::size_t i = 0; i < remaining; ++i) {
        buffer[i] = line[i];
      }

      buffer_size = remaining;
    }
  }

  return cores_read == snapshots.size();
}

void CpuLoadMonitor::print_to_file(const AppConfig& config) {
  std::ofstream file(config.filename, std::ios::app);

  if (!file.is_open()) {
    std::cerr << "Failed to open output file: " << config.filename << std::endl;
    return;
  }

  std::cout << std::endl;
  std::cout << "Logging CPU load every " << config.interval << " seconds to "
            << config.filename << std::endl;

  std::cout << "Press 'q' to stop logging." << std::endl;

  struct pollfd stdin_poll{};
  stdin_poll.fd = STDIN_FILENO;
  stdin_poll.events = POLLIN;

  while (true) {
    int result = poll(&stdin_poll, 1, static_cast<int>(config.interval * 1000));

    if (result < 0) {
      std::cerr << "poll() failed." << std::endl;
      return;
    }

    if (result > 0 && (stdin_poll.revents & POLLIN)) {
      char input;

      std::cin >> input;

      std::cin.ignore(10000, '\n');

      if (input == 'q' || input == 'Q') {
        std::cout << "Logging stopped." << std::endl;
        return;
      }

      std::cout << "Unknown command. "
                << "Press 'q' to stop logging." << std::endl;
    }

    if (result == 0) {
      if (!read_snapshots(current_snapshots)) {
        std::cerr << "Failed to read CPU snapshot." << std::endl;
        continue;
      }

      file << "CPU Load:" << std::endl;

      for (std::size_t i = 0; i < previous_snapshots.size(); ++i) {
        const CpuSnapshot& previous = previous_snapshots[i];

        const CpuSnapshot& current = current_snapshots[i];

        unsigned long long previous_idle = previous.idle + previous.iowait;

        unsigned long long current_idle = current.idle + current.iowait;

        unsigned long long previous_total =
            previous.user + previous.nice + previous.system + previous.idle +
            previous.iowait + previous.irq + previous.softirq + previous.steal;

        unsigned long long current_total =
            current.user + current.nice + current.system + current.idle +
            current.iowait + current.irq + current.softirq + current.steal;

        unsigned long long total_delta = current_total - previous_total;

        unsigned long long idle_delta = current_idle - previous_idle;

        double load = 0.0;

        if (total_delta > 0) {
          load = 100.0 * (static_cast<double>(total_delta - idle_delta) /
                          static_cast<double>(total_delta));
        }

        file << "  CPU" << i << ": " << std::fixed << std::setprecision(2)
             << std::setw(6) << load << "%" << std::endl;
      }

      file << std::endl;
      file.flush();

      previous_snapshots = current_snapshots;
    }
  }
}