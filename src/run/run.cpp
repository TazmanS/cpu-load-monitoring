#include "run.hpp"

#include <iostream>

#include "../cli/cli.hpp"

void run(CLI& cli) {
  std::cout << "Run project" << std::endl;
  std::cout << std::endl;

  while (true) {
    cli.show_menu();
  }
}