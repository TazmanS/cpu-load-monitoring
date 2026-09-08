#pragma once

#include <cstddef>
#include <vector>

#include "../config/app_config.hpp"
#include "cpu_snapshot.hpp"

class CpuLoadMonitor {
 public:
  ~CpuLoadMonitor();

  bool init();
  void show_load();
  void print_to_file(const AppConfig& config);

 private:
  bool read_snapshots(std::vector<CpuSnapshot>& snapshots);

  std::vector<CpuSnapshot> previous_snapshots;
  std::vector<CpuSnapshot> current_snapshots;

  int stat_fd = -1;

  static constexpr std::size_t BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
};