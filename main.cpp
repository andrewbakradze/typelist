#include "typelist.h"

#include <iostream>

int main() {
  using namespace ct::tl;

  using Types = TypeList<int, double, char>;
  using Extended = concat<Types, TypeList<bool>>;
  using Nested = TypeList<int, TypeList<double, char>>;
  using Flat = flatten<Nested>;

  static_assert(count<Types> == 3);
  static_assert(contains<double, Types>);
  static_assert(index_of_unique<char, Types> == 2);
  static_assert(count<Extended> == 4);
  static_assert(count<Flat> == 3);

  std::cout << std::boolalpha;
  std::cout << "Type count: " << count<Types> << '\n';
  std::cout << "Contains double: " << contains<double, Types> << '\n';
  std::cout << "Contains bool: " << contains<bool, Types> << '\n';
  std::cout << "Index of char: " << index_of_unique<char, Types> << '\n';
  std::cout << "Extended count: " << count<Extended> << '\n';
  std::cout << "Flattened count: " << count<Flat> << '\n';
}
