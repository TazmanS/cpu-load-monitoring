#include "cpu_load_monitor.hpp"

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

  if (!read_snapshots(previous_snapshots)) {
    std::cerr << "Failed to read initial CPU snapshot." << std::endl;
    return false;
  }

  return true;
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
  std::ifstream file("/proc/stat");

  if (!file.is_open()) {
    return false;
  }

  std::string cpu_name;
  std::size_t cores_read = 0;

  while (file >> cpu_name) {
    if (cpu_name == "cpu") {
      file.ignore(10000, '\n');
      continue;
    }

    if (cpu_name.rfind("cpu", 0) != 0) {
      break;
    }

    std::size_t core_index = std::stoul(cpu_name.substr(3));

    if (core_index >= snapshots.size()) {
      file.ignore(10000, '\n');
      continue;
    }

    CpuSnapshot& snapshot = snapshots[core_index];

    if (!(file >> snapshot.user >> snapshot.nice >> snapshot.system >>
          snapshot.idle >> snapshot.iowait >> snapshot.irq >>
          snapshot.softirq >> snapshot.steal >> snapshot.guest >>
          snapshot.guest_nice)) {
      return false;
    }

    ++cores_read;
  }

  return cores_read == snapshots.size();
}
