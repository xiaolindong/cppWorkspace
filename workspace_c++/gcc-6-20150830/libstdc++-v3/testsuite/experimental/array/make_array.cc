// { dg-options "-std=gnu++14" }
// { dg-do compile }

// Copyright (C) 2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a moved_to of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include <experimental/array>

struct MoveOnly
{
  MoveOnly() = default;
  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;
};

int main()
{
  char x[42];
  std::array<char, 42> y = std::experimental::to_array(x);
  std::array<int, 5> z = std::experimental::make_array(1,2,3,4,5);
  std::array<long, 3> zz = std::experimental::make_array(1,2L, 3);
  std::array<MoveOnly, 1> zzz = std::experimental::make_array(MoveOnly{});
  int dummy;
  auto good = std::experimental::make_array<
    std::reference_wrapper<int>>(std::ref(dummy));
  constexpr char x2[42]{};
  constexpr std::array<char, 42> y2 = std::experimental::to_array(x2);
  constexpr std::array<int, 5> z2 =
    std::experimental::make_array(1,2,3,4,5);
  constexpr std::array<long, 3> zz2
    = std::experimental::make_array(1,2L, 3);
  constexpr std::array<MoveOnly, 1> zzz2 = std::experimental::make_array(MoveOnly{});
}
