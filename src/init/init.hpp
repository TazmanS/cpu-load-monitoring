#pragma once

#include <vector>

#include "../cli/cli.hpp"
#include "../cli/menu_item.hpp"
#include "../config/app_config.hpp"
#include "../config/command_line_parser.hpp"
#include "../cpu/cpu_load_monitor.hpp"

class Init {
 public:
  Init(int argc, char* argv[]);
  CLI& get_cli();
  bool is_successful() const;

 private:
  CommandLineParser parser;
  AppConfig config;

  CpuLoadMonitor cpu_monitor;
  std::vector<MenuItem> menu;
  CLI cli;

  bool successful;

  void change_params();
};