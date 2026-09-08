#pragma once

#include <vector>

#include "cpu_snapshot.hpp"

class CpuLoadMonitor {
 public:
  bool init();
  void show_load();

 private:
  bool read_snapshots(std::vector<CpuSnapshot>& snapshots);
  std::vector<CpuSnapshot> previous_snapshots;
  std::vector<CpuSnapshot> current_snapshots;
};