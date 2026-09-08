#pragma once

#include <vector>

#include "../config/app_config.hpp"
#include "cpu_snapshot.hpp"

class CpuLoadMonitor {
 public:
  bool init();
  void show_load();
  void print_to_file(const AppConfig& config);

 private:
  bool read_snapshots(std::vector<CpuSnapshot>& snapshots);
  std::vector<CpuSnapshot> previous_snapshots;
  std::vector<CpuSnapshot> current_snapshots;
};