#pragma once

#include <gmock/gmock.h>

#include "Filesystem.h"

namespace vanadium {
namespace ut {

class FilesystemMock : public core::FsDirectory {
 public:
  MOCK_METHOD(bool, WriteFile, (std::string_view, std::string_view), (const, final));
  MOCK_METHOD(std::optional<std::string_view>, ReadFile, (std::string_view, FileContentsAllocator), (const, final));
};

}  // namespace ut
}  // namespace vanadium
