#pragma once

#include "app_config.hpp"

class CommandLineParser {
 public:
  AppConfig parse(int argc, char* argv[]);
};