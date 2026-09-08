#pragma once

#include <vector>

#include "menu_item.hpp"

class CLI {
 public:
  CLI(const std::vector<MenuItem>& menu);
  void show_menu();

 private:
  const std::vector<MenuItem>& menu;
};