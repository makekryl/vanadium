#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "FunctionRef.h"

namespace vanadium::core {

class Filesystem {
 public:
  virtual ~Filesystem() {}

  using FileContentsAllocator = lib::FunctionRef<char*(std::size_t)>;

  [[nodiscard]] virtual std::optional<std::string_view> ReadFile(std::string_view path,
                                                                 FileContentsAllocator alloc) const = 0;
  [[nodiscard]] virtual bool WriteFile(std::string_view path, std::string_view contents) const = 0;
};

class VirtualFS : public Filesystem {
 public:
  [[nodiscard]] std::optional<std::string_view> ReadFile(std::string_view path,
                                                         FileContentsAllocator alloc) const final;
  [[nodiscard]] bool WriteFile(std::string_view path, std::string_view contents) const final;

  [[nodiscard]] std::optional<std::string> Resolve(std::string_view path) const;
  void AddSearchPath(std::string_view path);

 private:
  std::vector<std::string> search_paths_;
};

}  // namespace vanadium::core
