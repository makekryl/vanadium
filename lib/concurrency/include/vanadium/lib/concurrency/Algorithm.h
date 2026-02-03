#pragma once

#include <iterator>

#include "vanadium/lib/concurrency/TaskGroup.h"

namespace vanadium::lib::concurrency {

template <typename Iterator, typename F>
  requires std::forward_iterator<Iterator>
void ParallelFor(Iterator begin, Iterator end, F&& f) {
  TaskGroup group;

  for (auto it = begin; it != end; it++) {
    group.Run([it, &f]() {
      f(*it);
    });
  }

  group.Wait();
}

template <std::ranges::forward_range R, typename F>
void ParallelFor(R&& range, F&& f) {
  ParallelFor(std::ranges::begin(range), std::ranges::end(range), std::forward<F>(f));
}

}  // namespace vanadium::lib::concurrency
