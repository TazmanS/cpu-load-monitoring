#pragma once

#include <vector>

#include "menu_item.hpp"

class CLI {
 public:
  CLI(const std::vector<MenuItem>& menu);
  void show_menu();
  void back();

 private:
  const std::vector<MenuItem>& menu;
  std::vector<std::size_t> path;
};