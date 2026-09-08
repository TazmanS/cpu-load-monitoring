#include "init.hpp"

#include <iostream>

void printToFile() { std::cout << "Print to file" << std::endl; }

void exitProgram() { std::cout << "Exit" << std::endl; }

Init::Init(int argc, char* argv[])
    : config(parser.parse(argc, argv)),
      menu{{1, "Show CPU load", [this]() { cpu_monitor.show_load(); }},
           {2,
            "Print to file",
            {},
            {{1, "Start logging",
              [this]() { cpu_monitor.print_to_file(config); }},
             {2, "Change parameters", [this]() { change_params(); }},
             {3, "Back", [this]() { cli.back(); }}}},
           {3, "Exit", [this]() { cli.stop(); }}},
      cli(menu),
      successful(false) {
  successful = cpu_monitor.init();
};

CLI& Init::get_cli() { return cli; }

bool Init::is_successful() const { return successful; }

void Init::change_params() {
  std::cout << std::endl;
  std::cout << "Current parameters:" << std::endl;
  std::cout << "  Interval: " << config.interval << " seconds" << std::endl;

  std::cout << "  File: " << config.filename << std::endl;

  std::cout << std::endl;

  std::cout << "Enter new interval (1-10 seconds): ";

  unsigned int interval;

  if (!(std::cin >> interval)) {
    std::cin.clear();
    std::cin.ignore(10000, '\n');

    std::cout << "Invalid interval." << std::endl;
    return;
  }

  if (interval < 1 || interval > 10) {
    std::cout << "Invalid interval. "
              << "Interval must be between 1 and 10 seconds." << std::endl;

    return;
  }

  std::cout << "Enter new file name: ";

  std::cin.ignore(10000, '\n');

  std::string filename;
  std::getline(std::cin, filename);

  if (filename.empty()) {
    std::cout << "Invalid file name." << std::endl;

    return;
  }

  config.interval = interval;
  config.filename = filename;

  std::cout << std::endl;
  std::cout << "Parameters updated." << std::endl;
}