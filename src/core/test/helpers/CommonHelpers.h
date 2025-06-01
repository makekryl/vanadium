#include <gtest/gtest.h>

namespace vanadium {
namespace ut {

template <class ParamType>
std::string ExplicitParametrizedTestDisplayName(const testing::TestParamInfo<ParamType>& info) {
  return info.param.test_name;
}

}  // namespace ut
}  // namespace vanadium
