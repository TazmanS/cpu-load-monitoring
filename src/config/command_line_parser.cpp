#include "command_line_parser.hpp"

#include <iostream>
#include <string>

AppConfig CommandLineParser::parse(int argc, char* argv[]) {
  AppConfig config;

  if (argc == 1) {
    return config;
  }

  if (argc != 3) {
    std::cout << "Invalid number of arguments. "
              << "Using default parameters." << std::endl;

    return config;
  }

  try {
    std::string interval_argument = argv[1];

    std::size_t pos = 0;
    unsigned long interval = std::stoul(interval_argument, &pos);

    if (pos != interval_argument.size() || interval < 1) {
      std::cout << "Invalid interval. "
                << "Using default parameters." << std::endl;

      return config;
    }

    config.interval = static_cast<unsigned int>(interval);
    config.filename = argv[2];
  } catch (const std::exception&) {
    std::cout << "Invalid interval. "
              << "Interval must be between 1 and 10 seconds. "
              << "Using default parameters." << std::endl;

    return config;
  }

  return config;
}