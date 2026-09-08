#include "cli.hpp"

#include <iostream>
#include <vector>

CLI::CLI(const std::vector<MenuItem>& menu) : menu(menu) {
  path.reserve(MAX_PATH_SIZE);
}

void CLI::show_menu() {
  const std::vector<MenuItem>* current_menu = &menu;

  for (std::size_t index : path) {
    current_menu = &(*current_menu)[index].children;
  }

  for (const auto& item : *current_menu) {
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

  for (std::size_t index = 0; index < current_menu->size(); ++index) {
    const MenuItem& item = (*current_menu)[index];

    if (item.key == choice) {
      if (item.callback) {
        item.callback();
      }

      if (!item.children.empty() && path.size() < MAX_PATH_SIZE) {
        path.push_back(index);
      }

      return;
    }
  }

  std::cout << "Invalid option." << std::endl;
  std::cout << std::endl;
}

void CLI::back() {
  if (!path.empty()) {
    path.pop_back();
  }
}

void CLI::stop() { running = false; }

bool CLI::is_running() const { return running; }