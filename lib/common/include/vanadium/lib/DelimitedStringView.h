#pragma once

#include <cctype>
#include <ranges>
#include <string_view>

namespace vanadium::lib {

template <char Delimiter>
class DelimitedStringView {
 public:
  DelimitedStringView(std::string_view s = {}) : s_(std::move(s)) {}

  constexpr auto range() {
    return s_ | std::views::split(Delimiter) | std::views::transform([](auto segment) {
             const auto is_space_or_newline = [](char c) {
               return std::isspace(c) || (c == '\n');
             };
             auto view = std::string_view(segment.begin(), segment.end())  //
                         | std::views::drop_while(is_space_or_newline)     //
                         | std::views::reverse                             //
                         | std::views::drop_while(is_space_or_newline)     //
                         | std::views::reverse;                            //
             return std::string_view(view.begin().base().base(), view.end().base().base());
           });
  }

  constexpr std::string_view sv() {
    return s_;
  }

 private:
  std::string_view s_;
};

}  // namespace vanadium::lib
