#include <iostream>

#include "init/init.hpp"
#include "run/run.hpp"

int main(int argc, char* argv[]) {
  std::cout << "CPU Load Monitor started" << std::endl;

  Init init(argc, argv);

  if (!init.is_successful()) {
    return 1;
  }

  run(init.get_cli());

  return 0;
}