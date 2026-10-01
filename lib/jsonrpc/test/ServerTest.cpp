#include <gtest/gtest.h>

#include <utility>

#include "vanadium/lib/jsonrpc/Common.h"
#include "vanadium/lib/jsonrpc/Server.h"

using namespace vanadium::lib::jsonrpc;

namespace stubs {
struct Context {
  int x{0};
};
struct Params {
  int a, b, c;
};
struct Result {
  Params p;
  int x, y, z;
};
}  // namespace stubs

TEST(ServerTest, RequestRegistration) {
  const auto handler = [](stubs::Context&, const stubs::Params&) -> ExpectedResult<stubs::Result> {
    return stubs::Result{};
  };

  Server<stubs::Context> srv;
  EXPECT_FALSE(srv.IsBound("request"));

  srv.Bind<+handler>("request");
  EXPECT_TRUE(srv.IsBound("request"));

  srv.Unbind("request");
  EXPECT_FALSE(srv.IsBound("request"));
}

TEST(ServerTest, NotificationRegistration) {
  const auto handler = [](stubs::Context&, const stubs::Params&) -> void {};

  Server<stubs::Context> srv;
  EXPECT_FALSE(srv.IsBound("notification"));

  srv.Bind<+handler>("notification");
  EXPECT_TRUE(srv.IsBound("notification"));

  srv.Unbind("notification");
  EXPECT_FALSE(srv.IsBound("notification"));
}

TEST(ServerTest, RequestInvocation) {
  const auto handler = [](stubs::Context& ctx, const stubs::Params& params) -> ExpectedResult<stubs::Result> {
    ctx.x = 42;
    return stubs::Result{.p = params, .x = 3, .y = 2, .z = 1};
  };

  stubs::Context ctx{.x = 0};
  std::string buf;

  Server<stubs::Context> srv;
  srv.Bind<+handler>("example");
  ASSERT_TRUE(srv.IsBound("example"));

  const auto res = srv.Call(ctx, buf, R"({
    "jsonrpc": "2.0",
    "method": "example",
    "params": {
      "a": 1,
      "b": 2,
      "c": 3
    },
    "id": 1
  })");
  EXPECT_EQ(res, R"({"jsonrpc":"2.0","result":{"p":{"a":1,"b":2,"c":3},"x":3,"y":2,"z":1},"id":1})");
  EXPECT_EQ(ctx.x, 42);
}

TEST(ServerTest, NotificationInvocation) {
  const auto handler = [](stubs::Context& ctx, const stubs::Params& params) -> void {
    ctx.x = params.a + params.b + params.c;
  };

  stubs::Context ctx{.x = 0};
  std::string buf;

  Server<stubs::Context> srv;
  srv.Bind<+handler>("example");
  ASSERT_TRUE(srv.IsBound("example"));

  const auto res = srv.Call(ctx, buf, R"({
    "jsonrpc": "2.0",
    "method": "example",
    "params": {
      "a": 1,
      "b": 2,
      "c": 3
    }
  })");
  EXPECT_EQ(res, std::nullopt);
  EXPECT_EQ(ctx.x, 6);
}

TEST(ServerTest, RequestParsingError) {
  const auto handler = [](stubs::Context&, const stubs::Params&) -> ExpectedResult<stubs::Result> {
    return stubs::Result{};
  };

  stubs::Context ctx{};
  std::string buf;

  Server<stubs::Context> srv;
  srv.Bind<+handler>("example");
  ASSERT_TRUE(srv.IsBound("example"));

  const auto res = srv.Call(ctx, buf, "this won't be parsed");
  EXPECT_EQ(
      res,
      R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error","data":"1:2: syntax_error\n   this won't be parsed\n    ^"},"id":-1})");
  // ^note: the expectation is dependent on the glaze behaviour
}

TEST(ServerTest, RequestInvocationError) {
  const auto handler = [](stubs::Context&, const stubs::Params&) -> ExpectedResult<stubs::Result> {
    return std::unexpected{Error{
        .code = ErrorCode::kInternal,  // reminder: it's hardcoded in the expectation to not to bother with format
        .data = std::nullopt,
        .message = "Error message",
    }};
  };

  stubs::Context ctx{};
  std::string buf;

  Server<stubs::Context> srv;
  srv.Bind<+handler>("example");
  ASSERT_TRUE(srv.IsBound("example"));

  const auto res = srv.Call(ctx, buf, R"({
    "jsonrpc": "2.0",
    "method": "example",
    "params": {
      "a": 1,
      "b": 2,
      "c": 3
    },
    "id": 1
  })");
  EXPECT_EQ(res, R"({"jsonrpc":"2.0","error":{"code":-32603,"message":"Error message"},"id":1})");
}
