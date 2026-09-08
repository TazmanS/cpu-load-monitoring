#pragma once

#include <functional>
#include <string>
#include <vector>

struct MenuItem {
  int key;
  std::string title;
  std::function<void()> callback;
  std::vector<MenuItem> children;
};