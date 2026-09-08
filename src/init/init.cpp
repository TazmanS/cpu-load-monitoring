#include "init.hpp"

#include <iostream>

void printToFile() { std::cout << "Print to file" << std::endl; }

void exitProgram() { std::cout << "Exit" << std::endl; }

Init::Init(int argc, char* argv[])
    : menu{{1, "Show CPU load", [this]() { cpu_monitor.show_load(); }},
           {2, "Print to file", printToFile},
           {3, "Exit", exitProgram}},
      cli(menu),
      successful(false) {
  successful = cpu_monitor.init();
};

CLI& Init::get_cli() { return cli; }

bool Init::is_successful() const { return successful; }