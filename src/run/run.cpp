#include "run.hpp"

#include <iostream>

#include "../cli/cli.hpp"

void run(CLI& cli) {
  std::cout << "Run project" << std::endl;
  std::cout << std::endl;

  while (cli.is_running()) {
    cli.show_menu();
  }
}