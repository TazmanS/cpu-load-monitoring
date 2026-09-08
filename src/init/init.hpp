#pragma once

#include <vector>

#include "../cli/cli.hpp"
#include "../cli/menu_item.hpp"
#include "../cpu/cpu_load_monitor.hpp"

class Init {
 public:
  Init(int argc, char* argv[]);
  CLI& get_cli();
  bool is_successful() const;

 private:
  CpuLoadMonitor cpu_monitor;
  std::vector<MenuItem> menu;
  CLI cli;
  bool successful;
};