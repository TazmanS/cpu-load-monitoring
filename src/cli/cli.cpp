#include "cli.hpp"

#include <iostream>

CLI::CLI(const std::vector<MenuItem>& menu) : menu(menu) {}

void CLI::show_menu() {
  for (const auto& item : menu) {
    std::cout << item.key << ". " << item.title << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Select option: ";

  int choice;

  if (!(std::cin >> choice)) {
    std::cin.clear();
    std::cin.ignore(10000, '\n');

    std::cout << "Invalid input." << std::endl;
    std::cout << std::endl;

    return;
  }

  for (const auto& item : menu) {
    if (item.key == choice) {
      if (item.callback) {
        item.callback();
      }

      return;
    }
  }

  std::cout << "Invalid option." << std::endl;
  std::cout << std::endl;
}