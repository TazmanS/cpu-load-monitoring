#pragma once

#include <vector>

#include "menu_item.hpp"

class CLI {
 public:
  CLI(const std::vector<MenuItem>& menu);
  void show_menu();
  void back();
  void stop();
  bool is_running() const;

 private:
  static constexpr std::size_t MAX_PATH_SIZE = 1;
  const std::vector<MenuItem>& menu;
  std::vector<std::size_t> path;
  bool running = true;
};