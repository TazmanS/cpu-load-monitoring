#pragma once

#include <string>

struct AppConfig {
  unsigned int interval = 5;
  std::string filename = "cpu_load.txt";
};